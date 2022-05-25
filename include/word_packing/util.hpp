/**
 * word_packing/util.hpp
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

#ifndef _WORD_PACKING_UTIL_HPP
#define _WORD_PACKING_UTIL_HPP

#include <bit>
#include <concepts>
#include <limits>

#include "internal/util.hpp"

namespace word_packing {

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

}

#endif
