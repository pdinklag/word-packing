/**
 * word_packing/internal/int_ref.hpp
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

#ifndef _WORD_PACKING_INTERNAL_INT_REF_HPP
#define _WORD_PACKING_INTERNAL_INT_REF_HPP

#include <cstddef>
#include <cstdint>

namespace word_packing::internal {

template<typename IntContainer>
struct IntRef {
    IntContainer* container;
    size_t index;

    IntRef(IntContainer& _container, size_t _index) : container(&_container), index(_index) {
    }

    IntRef(const IntRef&) = default;
    IntRef(IntRef&&) = default;
    
    IntRef& operator=(const IntRef&) = delete; // ambiguity with value assignment - use explicit casts!
    IntRef& operator=(IntRef&&) = delete; // ambiguity with value assignment - use explicit casts!

    operator uintmax_t() const { return container->get(index); }
    void operator=(uintmax_t x) { container->set(index, x); }

    bool operator==(IntRef const&) const = default;
    bool operator!=(IntRef const&) const = default;
};

template<typename IntContainer>
struct ConstIntRef {
    IntContainer const* container;
    size_t index;

    ConstIntRef(IntContainer const& _container, size_t _index) : container(&_container), index(_index) {
    }

    ConstIntRef(const ConstIntRef&) = default;
    ConstIntRef(ConstIntRef&&) = default;
    ConstIntRef& operator=(const ConstIntRef&) = default;
    ConstIntRef& operator=(ConstIntRef&&) = default;

    operator uintmax_t() const { return container->get(index); }

    bool operator==(ConstIntRef const&) const = default;
    bool operator!=(ConstIntRef const&) const = default;
};

}

#endif
