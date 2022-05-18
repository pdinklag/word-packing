#ifndef _PACKED_INT_VECTOR_HPP
#define _PACKED_INT_VECTOR_HPP

#include "int_container_helpers.hpp"

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace pdinklag {

using namespace word_packing_internals;

/**
 * \brief Vector of packed arbitrary-width (unsigned) integers (width known only at runtime)
 * 
 * This class packs integers of arbitrary bit width into a consecutive array of words, which can be used to save space.
 * However, accessing packed integers is substantially slower than aligned accesses due to the required arithmetics.
 * 
 * Use this class if the width of the contained integer is known only at runtime.
 * If the width is already known at compile time, use \ref PackedFixedIntVector to utilize a speedup through compiler optimizations.
 * 
 * The supported bit widths range from 1 to the width of the pack word type minus one.
 * 
 * \tparam PackWord the unsigned integer types to pack words into
 */
template<std::unsigned_integral PackWord>
class PackedIntVector {
private:
    static constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;

    size_t size_;
    size_t capacity_;
    size_t width_;
    size_t mask_;
    std::unique_ptr<PackWord[]> data_;

    using MyIntRef = IntRef<PackedIntVector>;
    using MyConstIntRef = ConstIntRef<PackedIntVector>;

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
    PackedIntVector(size_t size, size_t width) : size_(size), capacity_(size), width_(width), mask_(low_mask(width)) {
        assert(width_ > 0);
        assert(width_ < PACK_WORD_BITS);
        data_ = allocate_pack_words<PackWord>(capacity_, width_);
    }

    /**
     * \brief Retrieves a specific integer from the vector
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t get(size_t i) const {
        size_t const j = i * width_;
        size_t const a = j / PACK_WORD_BITS;                   // left border
        size_t const b = (j + width_ - 1ULL) / PACK_WORD_BITS; // right border

        // da is the distance of a's relevant bits from the left border
        size_t const da = j & (PACK_WORD_BITS - 1);

        // wa is the number of a's relevant bits
        size_t const wa = PACK_WORD_BITS - da;

        // get the wa highest bits from a
        uintmax_t const a_hi = data_[a] >> da;

        // get b (its high bits will be masked away below)
        // NOTE: we could save this step if we knew a == b,
        //       but the branch caused by checking that is too expensive
        uintmax_t const b_lo = data_[b];

        // combine
        return ((b_lo << wa) | a_hi) & mask_;
    }

    /**
     * \brief Writes a specific integer in the vector
     * 
     * \param i the index of the integer
     * \param x the value to write to the specified index
     */
    void set(size_t i, uintmax_t x) {
        uintmax_t const v = x & mask_; // make sure it fits...
        
        size_t const j = i * width_;
        size_t const a = j / PACK_WORD_BITS;                   // left border
        size_t const b = (j + width_ - 1ULL) / PACK_WORD_BITS; // right border
        if(a < b) {
            // the bits are the suffix of data_[a] and prefix of data_[b]
            size_t const da = j & (PACK_WORD_BITS - 1);
            size_t const wa = PACK_WORD_BITS - da;
            size_t const wb = width_ - wa;
            size_t const db = PACK_WORD_BITS - wb;

            // combine the da lowest bits from a and the wa lowest bits of v
            uintmax_t const a_lo = data_[a] & low_mask(da);
            uintmax_t const v_lo = v & low_mask(wa);
            data_[a] = (v_lo << da) | a_lo;

            // combine the db highest bits of b and the wb highest bits of v
            uintmax_t const b_hi = data_[b] >> wb;
            uintmax_t const v_hi = v >> wa;
            data_[b] = (b_hi << wb) | v_hi;
        } else {
            size_t const dl = j & (PACK_WORD_BITS - 1);
            uintmax_t const xa = data_[a];
            uintmax_t const mask_lo = low_mask(dl);
            uintmax_t const mask_hi = ~mask_lo << width_;
            data_[a] = (xa & mask_lo) | (v << dl) | (xa & mask_hi);
        }
    }

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
     * \brief Retrieves a specific integer from the vector
     * 
     * This function simply forwards to \ref get .
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t operator[](size_t i) const { return get(i); }

    /**
     * \brief Provides read/write access to a specific integer in the vector
     * 
     * \param i the index of the integer
     * \return a proxy allowing reading and writing
     */
    auto operator[](size_t i) { return IntRef(*this, i); }

    /**
     * \brief Returns an iterator to the first integer
     * 
     * \return an iterator to the first integer 
     */
    auto begin() { return IntIterator<MyIntRef> { IntRef(*this, 0) }; }

    /**
     * \brief Returns an iterator to right beyond the last integer
     * 
     * \return an iterator to right beyond the last integer
     */
    auto end() { return IntIterator<MyIntRef> { IntRef(*this, size_) }; }

    /**
     * \brief Returns a const iterator to the first integer
     * 
     * \return a const iterator to the first integer 
     */
    auto begin() const { return IntIterator<MyConstIntRef> { ConstIntRef(*this, 0) }; }

    /**
     * \brief Returns a const iterator to right beyond the last integer
     * 
     * \return a const iterator to right beyond the last integer
     */
    auto end() const { return IntIterator<MyConstIntRef> { ConstIntRef(*this, size_) }; }

    /**
     * \brief Provides read/write access to the first integer
     * 
     * \return a proxy allowing reading and writing
     */
    auto front() { return IntRef(*this, 0); }

    /**
     * \brief Returns the first integer
     * 
     * \return the first integer
     */
    uintmax_t front() const { return get(0); }

    /**
     * \brief Provides read/write access to the last integer
     * 
     * \return a proxy allowing reading and writing
     */
    auto back() { return IntRef(*this, size_ - 1); }

    /**
     * \brief Returns the last integer
     * 
     * \return the last integer
     */
    uintmax_t back() const { return get(size_ - 1); }

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

    /**
     * \brief Tests whether the vector is empty
     * 
     * \return true if the size equals zero
     * \return false otherwise
     */
    bool empty() const { return size_ == 0; }
};

}

#endif