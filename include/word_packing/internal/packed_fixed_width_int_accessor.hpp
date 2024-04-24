/**
 * word_packing/internal/packed_fixed_width_int_accessor.hpp
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

#ifndef _PACKED_FIXED_WIDTH_INT_ACCESSOR_HPP
#define _PACKED_FIXED_WIDTH_INT_ACCESSOR_HPP

#include "impl.hpp"
#include "int_ref.hpp"

namespace word_packing::internal {

template<size_t width_, WordPackEligible Pack = uintmax_t>
class PackedFixedWidthIntConstAccessor {
private:
    Pack const* data_;

public:
    PackedFixedWidthIntConstAccessor() : data_(nullptr) {}
    PackedFixedWidthIntConstAccessor(PackedFixedWidthIntConstAccessor&&) = default;
    PackedFixedWidthIntConstAccessor& operator=(PackedFixedWidthIntConstAccessor&&) = default;
    PackedFixedWidthIntConstAccessor(PackedFixedWidthIntConstAccessor const& other) = default;
    PackedFixedWidthIntConstAccessor& operator=(PackedFixedWidthIntConstAccessor const& other) = default;

    PackedFixedWidthIntConstAccessor(Pack const* data) : data_(data) {
    }

    uintmax_t get(size_t i) const { return internal::get<width_>(data_, i); }
    uintmax_t operator[](size_t i) const { return get(i); }
};

template<size_t width_, WordPackEligible Pack = uintmax_t>
class PackedFixedWidthIntAccessor {
private:
    Pack* data_;

public:
    PackedFixedWidthIntAccessor() : data_(nullptr) {}
    PackedFixedWidthIntAccessor(PackedFixedWidthIntAccessor&&) = default;
    PackedFixedWidthIntAccessor& operator=(PackedFixedWidthIntAccessor&&) = default;
    PackedFixedWidthIntAccessor(PackedFixedWidthIntAccessor const& other) = default;
    PackedFixedWidthIntAccessor& operator=(PackedFixedWidthIntAccessor const& other) = default;

    PackedFixedWidthIntAccessor(Pack* data) : data_(data) {
    }

    uintmax_t get(size_t i) const { return internal::get<width_>(data_, i); }
    void set(size_t i, uintmax_t x) { internal::set<width_>(data_, i, x); }

    uintmax_t operator[](size_t i) const { return get(i); }
    auto operator[](size_t i) { return IntRef(*this, i); }
};

}

#endif
