/**
 * word_packing.hpp
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

#ifndef _WORD_PACKING_HPP
#define _WORD_PACKING_HPP

#include "word_packing/internal/impl.hpp"

#include "word_packing/internal/packed_int_accessor.hpp"
#include "word_packing/internal/packed_fixed_width_int_accessor.hpp"

#include "word_packing/packed_int_vector.hpp"
#include "word_packing/packed_fixed_width_int_vector.hpp"

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
inline auto accessor(Pack const* data, size_t const width) { return internal::PackedIntConstAccessor<Pack>(data, width); }

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
inline auto accessor(Pack* data, size_t const width) { return internal::PackedIntAccessor<Pack>(data, width); }

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
inline auto accessor(Pack const* data) { return internal::PackedFixedWidthIntConstAccessor<width, Pack>(data); }

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
inline auto accessor(Pack* data) { return internal::PackedFixedWidthIntAccessor<width, Pack>(data); }

/**
 * \brief Allocates the required memory for an array of packed words of the given bit width and returns an accessor to it
 * 
 * \tparam Pack the word pack type
 * \param p the unique pointer to an array of word packs that will be assigned the allocated memory
 * \param num the capacity of the allocated array (in packed words)
 * \param width the bit width per packed word in the allocated array
 * \return auto an accessor to the packed word array
 */
template<WordPackEligible Pack>
auto alloc(std::unique_ptr<Pack[]>& p, size_t const num, size_t const width) {
    p = std::make_unique<Pack[]>(num_packs_required<Pack>(num, width));
    return accessor(p.get(), width);
}

/**
 * \brief Provides a read-only accessor to packed bits contained in the given pack buffer
 * 
 * This is equivalent to a packed-word accessor with bit width 1.
 * The access methods are specialized for this particular case and are faster than accessing integers of larger widths.
 * 
 * \tparam Pack the word pack type
 * \param data the word pack buffer to access
 * \return auto the accessor
 */
template<WordPackEligible Pack>
inline auto bit_accessor(Pack const* data) { return accessor<1>(data); }

/**
 * \brief Provides an accessor to packed bits contained in the given pack buffer
 * 
 * This is equivalent to a packed-word accessor with bit width 1.
 * The access methods are specialized for this particular case and are faster than accessing integers of larger widths.
 * 
 * \tparam Pack the word pack type
 * \param data the word pack buffer to access
 * \return auto the accessor
 */
template<WordPackEligible Pack>
inline auto bit_accessor(Pack* data) { return accessor<1>(data); }

/**
 * \brief Allocates the required memory for an array of packed bits and returns an accessor to it
 * 
 * \tparam Pack the word pack type
 * \param p the unique pointer to an array of bit packs that will be assigned the allocated memory
 * \param num the capacity of the allocated array (in bits)
 * \return auto an accessor to the packed bit array
 */
template<WordPackEligible Pack>
auto bit_alloc(std::unique_ptr<Pack[]>& p, size_t const num) {
    p = std::make_unique<Pack[]>(num_packs_required<Pack>(num, 1));
    return bit_accessor(p.get());
}

/**
 * \brief Convenience definition for bit vectors
 * 
 * This is equivalent to a packed integer vector of fixed width one.
 * The access methods are specialized for this particular case and are faster than accessing integers of larger widths.
 */
using BitVector = PackedFixedWidthIntVector<1>;

}

#endif
