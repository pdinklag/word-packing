#ifndef _PACKED_INT_VECTOR_HPP
#define _PACKED_INT_VECTOR_HPP

#include "int_container_impl.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <limits>

namespace pdinklag {

using namespace word_packing_internals;

/**
 * \brief Vector of packed arbitrary-width (unsigned) integers (width known only at runtime)
 * 
 * This class packs integers of arbitrary bit width into a consecutive array of words, which can be used to save space.
 * However, accessing packed integers is substantially slower than aligned accesses due to the required arithmetics.
 * 
 * Use this class if the width of the contained integers is known only at runtime.
 * 
 * The supported bit widths range from 1 to the width of the pack word type.
 * 
 * \tparam PackWord the unsigned integer types to pack words into
 */
template<std::unsigned_integral PackWord = uintmax_t>
class PackedIntVector : public IntContainer<PackedIntVector<PackWord>> {
private:
    static constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;
    static_assert(std::popcount(PACK_WORD_BITS) == 1, "the number of bits of a PACK_WORD_BITS must be a power of two");
    static constexpr size_t PACK_WORD_MAX = std::numeric_limits<PackWord>::max();

    size_t size_;
    size_t capacity_;
    size_t width_;
    size_t mask_;
    std::unique_ptr<PackWord[]> data_;

public:
    /**
     * \brief Constructs an empty vector of size zero
     * 
     */
    PackedIntVector() : size_(0), capacity_(0), width_(0), mask_(0) {
    }

    PackedIntVector(PackedIntVector&&) = default;
    PackedIntVector& operator=(PackedIntVector&&) = default;

    PackedIntVector(PackedIntVector const& other) { *this = other; }
    
    PackedIntVector& operator=(PackedIntVector const& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        width_ = other.width_;
        mask_ = other.mask_;
        data_ = allocate_pack_words<PackWord>(size_, width_);
        std::copy(other.data(), other.data() + pack_word_count<PackWord>(size_, width_), data());
        return *this;
    }

    /**
     * \brief Constructs a vector with the given size and width
     * 
     * Note that the vector's content is \em not initialized.
     * 
     * \param size the number of integers in the vector
     * \param width the width, in bits, of each integer
     */
    PackedIntVector(size_t size, size_t width) : size_(size), capacity_(size), width_(width), mask_(PACK_WORD_MAX >> (PACK_WORD_BITS - width)) {
        assert(width_ > 0);
        assert(width_ <= PACK_WORD_BITS);

        if(capacity_ > 0) {
            data_ = allocate_pack_words<PackWord>(capacity_, width_);
        }
    }

    /**
     * \brief Retrieves a specific integer from the vector
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t get(size_t i) const { return get_rt<PackWord>(i, data_.get(), width_, mask_); }

    /**
     * \brief Writes a specific integer in the vector
     * 
     * \param i the index of the integer
     * \param x the value to write to the specified index
     */
    void set(size_t i, uintmax_t x) { set_rt<PackWord>(i, x, data_.get(), width_, mask_); }

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
            PackedIntVector new_vec(capacity, width_);
            std::copy(data(), data() + pack_word_count<PackWord>(size_, width_), new_vec.data());
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
            PackedIntVector new_vec(size_, width_);
            std::copy(data(), data() + pack_word_count<PackWord>(size_, width_), new_vec.data());
            for(size_t i = 0; i < size_; i++) new_vec.set(i, get(i)); // TODO: copy pack words instead of individual integers
            *this = std::move(new_vec);
        }
    }

    /**
     * \brief Resizes the vector's size and integer width
     * 
     * The current items are retained, but in case the new size exceeds the current size, new items are \em not initialized.
     * In case the new width is lower than the current width, integers will be truncated.
     * 
     * \param size the new number of integers in the vector
     * \param width the new width, in bits, of each integer
     */
    void resize(size_t size, size_t width) {
        if(width == width_ && size <= capacity_) {
            // simply change the size
            size_ = size;
        } else {
            // allocate a new vector
            PackedIntVector new_vec(size, width);

            size_t const copy_num = std::min(size_, size);
            if(width_ == width) {
                // copy packs
                std::copy(data(), data() + pack_word_count<PackWord>(copy_num, width_), new_vec.data());
            } else {
                // the width has changed, copy integers one by one and possibly truncate
                for(size_t i = 0; i < copy_num; i++) new_vec.set(i, get(i));
            }

            *this = std::move(new_vec);
        }
    }

    /**
     * \brief Resizes the vector
     * 
     * The current items are retained, but in case the new size exceeds the current size, new items are \em not initialized.
     * 
     * \param size the new number of integers in the vector
     */
    void resize(size_t size) {
        resize(size, width_);
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
     * \brief Provides read and write access to the underlying array of pack words
     * 
     * Use with care: a packed word contains multiple integers from the vector and that modfying it may invalidate neighbouring vector entries.
     * 
     * \return the array of pack words
     */
    PackWord* data() { return data_.get(); }

    /**
     * \brief Provides read access to the underlying array of pack words
     * 
     * Note that these do not correspond directly to the integers contained in the vector.
     * 
     * \return the array of pack words
     */
    PackWord const* data() const { return data_.get(); }

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
