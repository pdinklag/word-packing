/**
 * packed_fixed_width_int_vector.hpp
 * part of pdinklag/word-packing
 * 
 * MIT License
 * 
 * Copyright (c) 2022 Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _PACKED_FIXED_INT_VECTOR_HPP
#define _PACKED_FIXED_INT_VECTOR_HPP

#include "int_container_impl.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace pdinklag {

using namespace word_packing_internals;

/**
 * \brief Vector of packed arbitrary-width (unsigned) integers (width known at compile time)
 * 
 * This class packs integers of arbitrary bit width into a consecutive array of words, which can be used to save space.
 * However, accessing packed integers is substantially slower than aligned accesses due to the required arithmetics.
 * 
 * Use this class if the width of the contained integers is already known at compile time.
 * 
 * The supported bit widths range from 1 to the width of the word pack type.
 * 
 * \tparam width_ the width per stored integer
 * \tparam Pack the unsigned integer type to pack words into
 */
template<size_t width_, WordPackEligible Pack = uintmax_t>
class PackedFixedWidthIntVector : public IntContainer<PackedFixedWidthIntVector<width_, Pack>> {
private:
    static constexpr size_t PACK_WORD_BITS = std::numeric_limits<Pack>::digits;
    static_assert(std::popcount(PACK_WORD_BITS) == 1, "the number of bits of a PACK_WORD_BITS must be a power of two");
    static constexpr size_t PACK_WORD_MAX = std::numeric_limits<Pack>::max();

    static_assert(width_ > 0);
    static_assert(width_ <= PACK_WORD_BITS);
    static constexpr size_t mask_ = PACK_WORD_MAX >> (PACK_WORD_BITS - width_);

    static constexpr bool aligned_ = (PACK_WORD_BITS % width_) == 0;

    size_t size_;
    size_t capacity_;
    std::unique_ptr<Pack[]> data_;

public:
    /**
     * \brief Constructs an empty vector of size zero
     * 
     */
    PackedFixedWidthIntVector() : size_(0), capacity_(0) {
    }

    PackedFixedWidthIntVector(PackedFixedWidthIntVector&&) = default;
    PackedFixedWidthIntVector& operator=(PackedFixedWidthIntVector&&) = default;

    PackedFixedWidthIntVector(PackedFixedWidthIntVector const& other) { *this = other; }
    
    PackedFixedWidthIntVector& operator=(PackedFixedWidthIntVector const& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = allocate_pack_words<Pack>(size_, width_);
        std::copy(other.data(), other.data() + pack_word_count<Pack>(size_, width_), data());
        return *this;
    }

    /**
     * \brief Constructs a vector with the given size and width
     * 
     * Note that the vector's content is \em not initialized.
     * 
     * \param size the number of integers in the vector
     */
    PackedFixedWidthIntVector(size_t size) : size_(size), capacity_(size) {
        if(capacity_ > 0) {
            data_ = allocate_pack_words<Pack>(capacity_, width_);
        }
    }

    /**
     * \brief Retrieves a specific integer from the vector
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t get(size_t i) const { return get_ct<Pack, width_>(i, data_.get()); }

    /**
     * \brief Writes a specific integer in the vector
     * 
     * \param i the index of the integer
     * \param x the value to write to the specified index
     */
    void set(size_t i, uintmax_t x) { set_ct<Pack, width_>(i, x, data_.get()); }

    /**
     * \brief Ensures that the vector's capacity fits at least the specified number of integers.
     * 
     * If the current capacity is already greater than or equal to the new capacity, nothing happens.
     * Use \ref shrink_to_fit to reduce the capacity to the current size.
     * 
     * \param capacity the minimum capacity
     */
    void reserve(size_t capacity) {
        if(capacity > capacity_) {
            // allocate a new vector and copy data
            PackedFixedWidthIntVector new_vec(capacity);
            std::copy(data(), data() + pack_word_count<Pack>(size_, width_), new_vec.data());
            new_vec.resize(size_); // this does nothing but set the size of the new vector
            *this = std::move(new_vec);
        }
    }

    /**
     * \brief Reduces the vector's capacity to its current size
     * 
     */
    void shrink_to_fit() {
        if(size_ < capacity_) {
            // allocate a new vector and copy data
            PackedFixedWidthIntVector new_vec(size_);
            std::copy(data(), data() + pack_word_count<Pack>(size_, width_), new_vec.data());
            for(size_t i = 0; i < size_; i++) new_vec.set(i, get(i)); // TODO: copy word packs instead of individual integers
            *this = std::move(new_vec);
        }
    }

    /**
     * \brief Resizes the vector's size and integer width
     * 
     * The current items are retained, but in case the new size exceeds the current size, new items are \em not initialized.
     * 
     * \param size the new number of integers in the vector
     */
    void resize(size_t size) {
        if(size <= capacity_) {
            // simply change the size
            size_ = size;
        } else {
            // allocate a new vector
            PackedFixedWidthIntVector new_vec(size);

            size_t const copy_num = std::min(size_, size);
            
            // copy packs
            std::copy(data(), data() + pack_word_count<Pack>(copy_num, width_), new_vec.data());

            *this = std::move(new_vec);
        }
    }

    /**
     * \brief Clears the vector, removing all elements
     * 
     * Note that this only resets the vector's size to zero, no memory is freed.
     */
    void clear() {
        size_ = 0;
    }

    /**
     * \brief Appends the specified integer to the back of the vector
     * 
     * If the vector's new size would exceed its capacity, the allocated memory region is grown by doubling the capacity.
     * 
     * \param x the integer to append
     */
    void push_back(uintmax_t const x) {
        if(size_ >= capacity_) {
            reserve(capacity_ ? 2 * capacity_ : 1);
        }
        set(size_++, x);
    }

    /**
     * \brief Appends the specified integer to the back of the vector
     * 
     * This is equivalent to using \ref push_back .
     * 
     * \param x the integer to append
     */
    void emplace_back(uintmax_t&& x) { push_back(x); }

    /**
     * \brief Removes the last integer from the vector, if any
     * 
     * This will not reduce the vector's capacity.
     */
    void pop_back() {
        if(size_) --size_;
    }

    /**
     * \brief Provides read and write access to the underlying array of word packs
     * 
     * Use with care: a word pack contains multiple integers from the vector and that modfying it may invalidate neighbouring vector entries.
     * 
     * \return the array of word packs
     */
    Pack* data() { return data_.get(); }

    /**
     * \brief Provides read access to the underlying array of word packs
     * 
     * Note that these do not correspond directly to the integers contained in the vector.
     * 
     * \return the array of word packs
     */
    Pack const* data() const { return data_.get(); }

    /**
     * \brief Reports the bit width of the contained integers
     *
     * \return the bit width of the contained integers
     */
    size_t width() const { return width_; }

    /**
     * \brief Reports the number of contained integers
     * 
     * \return the number of contained integers 
     */
    size_t size() const { return size_; }

    /**
     * \brief Reports the capacity of the vector
     * 
     * This equals the number of packed integers that the allocated memory region can currently fit.
     * 
     * \return the number of integers that fit into the allocated memory
     */
    size_t capacity() const { return capacity_; }
};

}

#endif
