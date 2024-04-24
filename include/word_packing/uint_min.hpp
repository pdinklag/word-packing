/**
 * word_packing/uint_min.hpp
 * part of pdinklag/word-packing
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage
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

#ifndef _WORD_PACKING_UINT_MIN_HPP
#define _WORD_PACKING_UINT_MIN_HPP

#include <cstdint>
#include <type_traits>

namespace word_packing {

/**
 * \brief The minimum unsigned integer type that consists of at least the specified number of bits
 * 
 * This only supports up to 64 bits.
 * 
 * \tparam bits the number of bits that the integer type must be able to fit
 */
template<unsigned bits>
using UintMin =
    typename std::conditional<(bits > 64), void,
        typename std::conditional<(bits > 32), uint64_t,
            typename std::conditional<(bits > 16), uint32_t,
                typename std::conditional<(bits > 8), uint16_t, uint8_t>::type
            >::type
        >::type
    >::type;

}

#endif
