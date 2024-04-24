/**
 * word_packing/internal/util.hpp
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

#ifndef _WORD_PACKING_INTERNAL_UTIL_HPP
#define _WORD_PACKING_INTERNAL_UTIL_HPP

#include <cstddef>
#include <cstdint>

namespace word_packing::internal {
    constexpr uintmax_t low_mask(size_t const bits) {
        return ~((UINTMAX_MAX << (bits - 1)) << 1); // nb: bits > 0 is assumed!
    }

    constexpr uintmax_t low_mask0(size_t const bits) {
        return ~(UINTMAX_MAX << bits); // nb: bits < max bits is assumed!
    }

    constexpr size_t idiv_ceil(size_t const a, size_t const b) {
        return ((a + b) - 1ULL) / b;
    }
}

#endif
