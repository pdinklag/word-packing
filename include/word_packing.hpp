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

#include "word_packing_impl.hpp"

#include "packed_int_accessor.hpp"
#include "packed_fixed_width_int_accessor.hpp"
#include "packed_int_vector.hpp"
#include "packed_fixed_width_int_vector.hpp"

namespace word_packing {

/**
 * \brief Provides a read-only accessor to packed words of the given bit width contained in the given pack buffer
 * 
 * Use this if the width is only known at runtime.
 * 
 * \tparam Pack the word pack type
 * \param data the word pack buffer to access
 * \param width the bit width per packed word
 * \return the accessor
 */
template<WordPackEligible Pack>
inline auto accessor(Pack const* data, size_t const width) { return internal::PackedIntConstAccessor(data, width); }

/**
 * \brief Provides an accessor to packed words of the given bit width contained in the given pack buffer
 * 
 * Use this if the width is only known at runtime.
 * 
 * \tparam Pack the word pack type
 * \param data the word pack buffer to access
 * \param width the bit width per packed word
 * \return the accessor
 */
template<WordPackEligible Pack>
inline auto accessor(Pack* data, size_t const width) { return internal::PackedIntAccessor(data, width); }

/**
 * \brief Provides a read-only accessor to packed words of the given bit width contained in the given pack buffer
 * 
 * Use this if the width is already known at compile time.
 * 
 * \tparam Pack the word pack type
 * \tparam width the bit width per packed word
 * \param data the word pack buffer to access
 * \return the accessor
 */
template<size_t width, WordPackEligible Pack>
inline auto accessor(Pack const* data) { return internal::PackedFixedWidthIntConstAccessor<width>(data); }

/**
 * \brief Provides an accessor to packed words of the given bit width contained in the given pack buffer
 * 
 * Use this if the width is already known at compile time.
 * 
 * \tparam Pack the word pack type
 * \tparam width the bit width per packed word
 * \param data the word pack buffer to access
 * \return the accessor
 */
template<size_t width, WordPackEligible Pack>
inline auto accessor(Pack* data) { return internal::PackedFixedWidthIntAccessor<width>(data); }

}

#endif