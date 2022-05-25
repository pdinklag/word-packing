/**
 * test_packed_int_vector_16.cpp
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <packed_int_vector.hpp>
#include <packed_fixed_width_int_vector.hpp>

#include <packed_int_access.hpp>
#include <packed_fixed_width_int_access.hpp>

namespace pdinklag::test::word_packing {

constexpr uint64_t MAGIC_NUMBER = 0xFEDCBA9876543210ULL;

TEST_SUITE("word_packing") {
    TEST_CASE("packed_int_vector") {
        PackedIntVector<> v(1, 64);
        v[0] = MAGIC_NUMBER;
        CHECK(v[0] == MAGIC_NUMBER);
    }

    TEST_CASE("packed_fixed_width_int_vector") {
        PackedFixedWidthIntVector<64> v(1);
        v[0] = MAGIC_NUMBER;
        CHECK(v[0] == MAGIC_NUMBER);
    }

    TEST_CASE("packed_int_access") {
        uint64_t data;
        PackedIntAccess v(&data, 64);
        v[0] = MAGIC_NUMBER;
        CHECK(v[0] == MAGIC_NUMBER);
    }

    TEST_CASE("packed_fixed_width_int_access") {
        uint64_t data;
        PackedFixedWidthIntAccess<64, decltype(data)> v(&data);
        v[0] = MAGIC_NUMBER;
        CHECK(v[0] == MAGIC_NUMBER);
    }
}

}
