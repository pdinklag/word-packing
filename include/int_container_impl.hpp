/**
 * int_container_impl.hpp
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

#ifndef _INT_CONTAINER_IMPL_HPP
#define _INT_CONTAINER_IMPL_HPP

#include "int_container_helpers.hpp"

namespace pdinklag::word_packing_internals {

/**
 * \brief Reads an integer from a packed container
 * 
 * This implementation is for the case where the width per integer is known only at runtime.
 * 
 * \tparam PackWord the pack word type
 * \param i the index of the integer to read
 * \param data the array of pack words
 * \param width the width per integer in the container
 * \param mask the precomputed mask for masking out the `width` low bits of an integer
 * \return the read integer
 */
template<std::unsigned_integral PackWord>
inline uintmax_t get_rt(size_t const i, PackWord const* data, size_t const width, size_t const mask) {
    constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;

    size_t const j = i * width;
    size_t const a = j / PACK_WORD_BITS;                   // left border
    size_t const b = (j + width - 1ULL) / PACK_WORD_BITS; // right border

    // da is the distance of a's relevant bits from the left border
    size_t const da = j & (PACK_WORD_BITS - 1);

    // wa is the number of a's relevant bits
    size_t const wa = PACK_WORD_BITS - da;

    // get the wa highest bits from a
    uintmax_t const a_hi = data[a] >> da;

    // get b (its high bits will be masked away below)
    // NOTE: we could save this step if we knew a == b,
    //       but the branch caused by checking that is too expensive
    uintmax_t const b_lo = data[b];

    // combine
    return ((b_lo << wa) | a_hi) & mask;
}

/**
 * \brief Reads an integer from a packed container
 * 
 * This implementation is optimized for the case where the width per integer is already known at compile time.
 * 
 * \tparam PackWord the pack word type
 * \tparam width the width per integer in the container
 * \param i the index of the integer to read
 * \param data the array of pack words
 * \return the read integer
 */
template<std::unsigned_integral PackWord, size_t width>
inline  uintmax_t get_ct(size_t const i, PackWord const* data) {
    constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;
    constexpr size_t PACK_WORD_MAX = std::numeric_limits<PackWord>::max();

    constexpr size_t mask = PACK_WORD_MAX >> (PACK_WORD_BITS - width);
    constexpr bool aligned = (PACK_WORD_BITS % width) == 0;

    size_t const j = i * width;
    size_t const a = j / PACK_WORD_BITS;                   // left border

    // da is the distance of a's relevant bits from the left border
    size_t const da = j & (PACK_WORD_BITS - 1);

    // get the wa highest bits from a
    uintmax_t const a_hi = data[a] >> da;

    if constexpr(aligned) {
        // if we're aligned, we don't need to consider the next pack
        return a_hi & mask;
    } else {
        size_t const b = (j + width - 1ULL) / PACK_WORD_BITS; // right border

        // wa is the number of a's relevant bits
        size_t const wa = PACK_WORD_BITS - da;

        // get b (its high bits will be masked away below)
        // NOTE: we could save this step if we knew a == b,
        //       but the branch caused by checking that is too expensive
        uintmax_t const b_lo = data[b];

        // combine
        return ((b_lo << wa) | a_hi) & mask;
    }
}

/**
 * \brief Writes an integer in a packed container
 * 
 * This implementation is for the case where the width per integer is known only at runtime.
 * 
 * \tparam PackWord the pack word type
 * \param i the index of the integer to read
 * \param x the value to write
 * \param data the array of pack words
 * \param width the width per integer in the container
 * \param mask the precomputed mask for masking out the `width` low bits of an integer
 * \return the read integer
 */
template<std::unsigned_integral PackWord>
inline void set_rt(size_t const i, uintmax_t const x, PackWord* data, size_t const width, size_t const mask) {
    constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;

    uintmax_t const v = x & mask; // make sure it fits...
    
    size_t const j = i * width;
    size_t const a = j / PACK_WORD_BITS;                   // left border
    size_t const b = (j + width - 1ULL) / PACK_WORD_BITS; // right border

    // get starting position of relevant bit block within data[a]
    size_t const da = j & (PACK_WORD_BITS - 1);
    assert(da < PACK_WORD_BITS);

    if(a == b) {
        // the bits are an infix of data[a]
        uintmax_t const xa = data[a];
        uintmax_t const mask_lo = low_mask0(da);
        uintmax_t const mask_hi = ~mask_lo << (width-1) << 1; // nb: the extra shift ensures that this works for width_ = 64
        data[a] = (xa & mask_lo) | (v << da) | (xa & mask_hi);
    } else {
        // the bits are the suffix of data[a] and prefix of data[b]
        size_t const wa = PACK_WORD_BITS - da;
        assert(wa > 0);
        assert(wa < width);
        size_t const wb = width - wa;

        // combine the da lowest bits from a and the wa lowest bits of v
        uintmax_t const a_lo = data[a] & low_mask0(da);
        uintmax_t const v_lo = v & low_mask(wa);
        data[a] = (v_lo << da) | a_lo;

        // combine the db highest bits of b and the wb highest bits of v
        uintmax_t const b_hi = data[b] >> wb;
        uintmax_t const v_hi = v >> wa;
        data[b] = (b_hi << wb) | v_hi;
    }
}

/**
 * \brief Writes an integer to a packed container
 * 
 * This implementation is optimized for the case where the width per integer is already known at compile time.
 * 
 * \tparam PackWord the pack word type
 * \tparam width the width per integer in the container
 * \param i the index of the integer to read
 * \param x the value to write
 * \param data the array of pack words
 * \return the read integer
 */
template<std::unsigned_integral PackWord, size_t width>
inline void set_ct(size_t const i, uintmax_t const x, PackWord* data) {
    constexpr size_t PACK_WORD_BITS = std::numeric_limits<PackWord>::digits;
    constexpr size_t PACK_WORD_MAX = std::numeric_limits<PackWord>::max();

    constexpr size_t mask = PACK_WORD_MAX >> (PACK_WORD_BITS - width);
    constexpr bool aligned = (PACK_WORD_BITS % width) == 0;

    uintmax_t const v = x & mask; // make sure it fits...
    
    size_t const j = i * width;
    size_t const a = j / PACK_WORD_BITS;                   // left border
    size_t const b = (j + width - 1ULL) / PACK_WORD_BITS; // right border

    // get starting position of relevant block within data[a]
    size_t const da = j & (PACK_WORD_BITS - 1);
    assert(da < PACK_WORD_BITS);

    if(aligned || a == b) {
        // the bits are an infix of data_[a]
        uintmax_t const xa = data[a];
        uintmax_t const mask_lo = low_mask0(da);
        uintmax_t const mask_hi = ~mask_lo << (width-1) << 1; // nb: the extra shift ensures that this works for width_ = 64
        data[a] = (xa & mask_lo) | (v << da) | (xa & mask_hi);
    } else {
        // the bits are the suffix of data_[a] and prefix of data[b]
        size_t const wa = PACK_WORD_BITS - da;
        assert(wa > 0);
        assert(wa < width);
        size_t const wb = width - wa;

        // combine the da lowest bits from a and the wa lowest bits of v
        uintmax_t const a_lo = data[a] & low_mask0(da);
        uintmax_t const v_lo = v & low_mask(wa);
        data[a] = (v_lo << da) | a_lo;

        // combine the db highest bits of b and the wb highest bits of v
        uintmax_t const b_hi = data[b] >> wb;
        uintmax_t const v_hi = v >> wa;
        data[b] = (b_hi << wb) | v_hi;
    }
}

}

#endif
