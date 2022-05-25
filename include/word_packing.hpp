/**
 * word_packing.hpp
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

#ifndef _WORD_PACKING_HPP
#define _WORD_PACKING_HPP

#include <bit>
#include <cassert>
#include <concepts>
#include <limits>

namespace word_packing::internal {
    constexpr uintmax_t low_mask0(size_t const bits) {
        return ~(UINTMAX_MAX << bits); // nb: bits < max bits is assumed!
    }

    constexpr size_t idiv_ceil(size_t const a, size_t const b) {
        return ((a + b) - 1ULL) / b;
    }
}

namespace word_packing {

/**
 * \brief Computes a bit mask for extracting the given number of low bits from an integer
 * 
 * \param bits the number of low bits to extract; must be greater than 0
 * \return a bit mask for extracting the given number of low bits from an integer
 */
constexpr uintmax_t low_mask(size_t const bits) {
    return ~((UINTMAX_MAX << (bits - 1)) << 1); // nb: bits > 0 is assumed!
}

/**
 * \brief Computes the number of packs required to store the given number of integers with the given bit width
 * 
 * \tparam Pack the word pack type
 * \param num the number of packed integers
 * \param width the bit width of each packed integer
 * \return the number of packs required for storage
 */
template<std::unsigned_integral Pack>
constexpr size_t num_packs_required(size_t const num, size_t const width) {
    return internal::idiv_ceil(num * width, std::numeric_limits<Pack>::digits);
}

/**
 * \brief Concept for types that are eligible as word packs
 * 
 * This requires the type to be an unsigned integral of bit width a power of two.
 * 
 * \tparam T the type in question
 */
template<typename T>
concept WordPackEligible =
    std::unsigned_integral<T> &&
    std::popcount(unsigned(std::numeric_limits<T>::digits)) == 1; // word packs must have width a power of two

/**
 * \brief Reads an integer from a packed container
 * 
 * \tparam Pack the word pack type
 * \param data the array of word packs
 * \param i the index of the integer to read
 * \param width the width per integer in the container
 * \param mask the precomputed mask for masking out the `width` low bits of an integer (\see low_mask)
 * \return the read integer
 */
template<WordPackEligible Pack>
inline uintmax_t get(Pack const* data, size_t const i, size_t const width, uintmax_t const mask) {
    constexpr size_t PACK_BITS = std::numeric_limits<Pack>::digits;

    size_t const j = i * width;
    size_t const a = j / PACK_BITS;                  // left border
    size_t const b = (j + width - 1ULL) / PACK_BITS; // right border

    // da is the distance of a's relevant bits from the left border
    size_t const da = j & (PACK_BITS - 1);

    // wa is the number of a's relevant bits
    size_t const wa = PACK_BITS - da;

    // get the wa highest bits from a
    uintmax_t const a_hi = data[a] >> da;

    // get b (its high bits will be masked away below)
    // NOTE: we could save this step if we knew a == b,
    //       but the branch caused by checking that is too expensive
    uintmax_t const b_lo = data[b];

    // combine
    return ((b_lo << wa) | a_hi) & mask;
}

template<WordPackEligible Pack>
inline uintmax_t get(Pack const* data, size_t const i, size_t const width) { return get(data, i, width, low_mask(width)); }

/**
 * \brief Reads an integer from a packed container
 * 
 * \tparam Pack the word pack type
 * \tparam width the width per integer in the container
 * \param data the array of word packs
 * \param i the index of the integer to read
 * \return the read integer
 */
template<size_t width, WordPackEligible Pack>
inline uintmax_t get(Pack const* data, size_t const i) {
    constexpr size_t PACK_BITS = std::numeric_limits<Pack>::digits;

    constexpr uintmax_t mask = low_mask(width);
    constexpr bool aligned = (PACK_BITS % width) == 0;

    size_t const j = i * width;
    size_t const a = j / PACK_BITS;                   // left border

    // da is the distance of a's relevant bits from the left border
    size_t const da = j & (PACK_BITS - 1);

    // get the wa highest bits from a
    uintmax_t const a_hi = data[a] >> da;

    if constexpr(aligned) {
        // if we're aligned, we don't need to consider the next pack
        return a_hi & mask;
    } else {
        size_t const b = (j + width - 1ULL) / PACK_BITS; // right border

        // wa is the number of a's relevant bits
        size_t const wa = PACK_BITS - da;

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
 * \tparam Pack the word pack type
 * \param data the array of word packs
 * \param i the index of the integer to read
 * \param x the value to write
 * \param width the width per integer in the container
 * \param mask the precomputed mask for masking out the `width` low bits of an integer (\see low_mask)
 * \return the read integer
 */
template<WordPackEligible Pack>
inline void set(Pack* data, size_t const i, uintmax_t const x, size_t const width, uintmax_t const mask) {
    constexpr size_t PACK_BITS = std::numeric_limits<Pack>::digits;

    uintmax_t const v = x & mask; // make sure it fits...
    
    size_t const j = i * width;
    size_t const a = j / PACK_BITS;                   // left border
    size_t const b = (j + width - 1ULL) / PACK_BITS; // right border

    // get starting position of relevant bit block within data[a]
    size_t const da = j & (PACK_BITS - 1);
    assert(da < PACK_BITS);

    if(a == b) {
        // the bits are an infix of data[a]
        uintmax_t const xa = data[a];
        uintmax_t const mask_lo = internal::low_mask0(da);
        uintmax_t const mask_hi = ~mask_lo << (width-1) << 1; // nb: the extra shift ensures that this works for width_ = 64
        data[a] = (xa & mask_lo) | (v << da) | (xa & mask_hi);
    } else {
        // the bits are the suffix of data[a] and prefix of data[b]
        size_t const wa = PACK_BITS - da;
        assert(wa > 0);
        assert(wa < width);
        size_t const wb = width - wa;

        // combine the da lowest bits from a and the wa lowest bits of v
        uintmax_t const a_lo = data[a] & internal::low_mask0(da);
        uintmax_t const v_lo = v & low_mask(wa);
        data[a] = (v_lo << da) | a_lo;

        // combine the db highest bits of b and the wb highest bits of v
        uintmax_t const b_hi = data[b] >> wb;
        uintmax_t const v_hi = v >> wa;
        data[b] = (b_hi << wb) | v_hi;
    }
}

template<WordPackEligible Pack>
inline void set(Pack* data, size_t const i, uintmax_t const x, size_t const width) { set(data, i, x, width, low_mask(width)); }

/**
 * \brief Writes an integer to a packed container
 * 
 * \tparam Pack the word pack type
 * \tparam width the width per integer in the container
 * \param data the array of word packs
 * \param i the index of the integer to read
 * \param x the value to write
 * \return the read integer
 */
template<size_t width, WordPackEligible Pack>
inline void set(Pack* data, size_t const i, uintmax_t const x) {
    constexpr size_t PACK_BITS = std::numeric_limits<Pack>::digits;
    constexpr size_t PACK_MAX = std::numeric_limits<Pack>::max();

    constexpr uintmax_t mask = low_mask(width);
    constexpr bool aligned = (PACK_BITS % width) == 0;

    uintmax_t const v = x & mask; // make sure it fits...
    
    size_t const j = i * width;
    size_t const a = j / PACK_BITS;                   // left border
    size_t const b = (j + width - 1ULL) / PACK_BITS; // right border

    // get starting position of relevant block within data[a]
    size_t const da = j & (PACK_BITS - 1);
    assert(da < PACK_BITS);

    if(aligned || a == b) {
        // the bits are an infix of data_[a]
        uintmax_t const xa = data[a];
        uintmax_t const mask_lo = internal::low_mask0(da);
        uintmax_t const mask_hi = ~mask_lo << (width-1) << 1; // nb: the extra shift ensures that this works for width_ = 64
        data[a] = (xa & mask_lo) | (v << da) | (xa & mask_hi);
    } else {
        // the bits are the suffix of data_[a] and prefix of data[b]
        size_t const wa = PACK_BITS - da;
        assert(wa > 0);
        assert(wa < width);
        size_t const wb = width - wa;

        // combine the da lowest bits from a and the wa lowest bits of v
        uintmax_t const a_lo = data[a] & internal::low_mask0(da);
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
