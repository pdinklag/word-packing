/**
 * word_packing/internal/packed_int_accessor.hpp
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

#ifndef _WORD_PACKING_INTERNAL_PACKED_INT_ACCESSOR_HPP
#define _WORD_PACKING_INTERNAL_PACKED_INT_ACCESSOR_HPP

#include "impl.hpp"
#include "int_ref.hpp"

namespace word_packing::internal {

template<WordPackEligible Pack = uintmax_t>
class PackedIntConstAccessor {
private:
    Pack const* data_;
    size_t width_;
    size_t mask_;

public:
    PackedIntConstAccessor() : data_(nullptr), width_(0), mask_(0) {}
    PackedIntConstAccessor(PackedIntConstAccessor&&) = default;
    PackedIntConstAccessor& operator=(PackedIntConstAccessor&&) = default;
    PackedIntConstAccessor(PackedIntConstAccessor const& other) = default;
    PackedIntConstAccessor& operator=(PackedIntConstAccessor const& other) = default;

    PackedIntConstAccessor(Pack const* data, size_t width) : data_(data), width_(width), mask_(internal::low_mask(width)) {
        assert(width_ > 0);
        assert(width_ <= std::numeric_limits<Pack>::digits);
    }

    uintmax_t get(size_t i) const { return internal::get<Pack>(data_, i, width_, mask_); }
    uintmax_t operator[](size_t i) const { return get(i); }
};

template<WordPackEligible Pack = uintmax_t>
class PackedIntAccessor {
private:
    Pack* data_;
    size_t width_;
    size_t mask_;

public:
    PackedIntAccessor() : data_(nullptr), width_(0), mask_(0) {}
    PackedIntAccessor(PackedIntAccessor&&) = default;
    PackedIntAccessor& operator=(PackedIntAccessor&&) = default;
    PackedIntAccessor(PackedIntAccessor const& other) = default;
    PackedIntAccessor& operator=(PackedIntAccessor const& other) = default;

    PackedIntAccessor(Pack* data, size_t width) : data_(data), width_(width), mask_(low_mask(width)) {
        assert(width_ > 0);
        assert(width_ <= std::numeric_limits<Pack>::digits);
    }

    uintmax_t get(size_t i) const { return internal::get(data_, i, width_, mask_); }
    void set(size_t i, uintmax_t x) { internal::set(data_, i, x, width_, mask_); }

    uintmax_t operator[](size_t i) const { return get(i); }
    auto operator[](size_t i) { return IntRef(*this, i); }
};

}

#endif
