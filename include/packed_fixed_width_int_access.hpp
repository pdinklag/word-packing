/**
 * packed_int_access.hpp
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

#ifndef _PACKED_FIXED_WIDTH_INT_ACCESS_HPP
#define _PACKED_FIXED_WIDTH_INT_ACCESS_HPP

#include "int_container_impl.hpp"

#include <algorithm>

namespace pdinklag {

using namespace word_packing_internals;

/**
 * \brief Access to packed arbitrary-width (unsigned) integers (width known only at runtime)
 * 
 * This class decorates a pointer to word packs with access to integers of arbitrary bit width.
 * Use this only if the width of the contained integers is known only at runtime.
 * 
 * The supported bit widths range from 1 to the width of the pack word type.
 * 
 * \tparam Pack the unsigned integer type to pack words into
 */
template<size_t width_, WordPackEligible Pack>
class PackedFixedWidthIntAccess : public IntContainer<PackedFixedWidthIntAccess<width_, Pack>> {
private:
    static_assert(width_ > 0, "width cannot be zero");
    static_assert(width_ <= std::numeric_limits<Pack>::digits, "word pack width must be at list the word width");

    Pack* data_;

public:
    /**
     * \brief Constructs an empty vector of size zero
     * 
     */
    PackedFixedWidthIntAccess() : data_(nullptr) {
    }

    PackedFixedWidthIntAccess(PackedFixedWidthIntAccess&&) = default;
    PackedFixedWidthIntAccess& operator=(PackedFixedWidthIntAccess&&) = default;

    PackedFixedWidthIntAccess(PackedFixedWidthIntAccess const& other) = default;
    PackedFixedWidthIntAccess& operator=(PackedFixedWidthIntAccess const& other) = default;

    /**
     * \brief Constructs a vector with the given size and width
     * 
     * Note that the vector's content is \em not initialized.
     * 
     * \param size the number of integers in the vector
     * \param width the width, in bits, of each integer
     */
    PackedFixedWidthIntAccess(Pack* data) : data_(data) {
    }

    /**
     * \brief Retrieves a specific integer from the vector
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t get(size_t i) const { return get_ct<Pack, width_>(i, data_); }

    /**
     * \brief Writes a specific integer in the vector
     * 
     * \param i the index of the integer
     * \param x the value to write to the specified index
     */
    void set(size_t i, uintmax_t x) { set_ct<Pack, width_>(i, x, data_); }

    /**
     * \brief Provides read and write access to the underlying array of word packs
     * 
     * Use with care: a word pack contains multiple integers from the vector and that modfying it may invalidate neighbouring vector entries.
     * 
     * \return the array of word packs
     */
    Pack* data() { return data_; }

    /**
     * \brief Provides read access to the underlying array of word packs
     * 
     * Note that these do not correspond directly to the integers contained in the vector.
     * 
     * \return the array of word packs
     */
    Pack const* const& data() const { return data_; }

    /**
     * \brief Reports the bit width of the contained integers
     *
     * \return the bit width of the contained integers
     */
    size_t width() const { return width_; }
};

}

#endif
