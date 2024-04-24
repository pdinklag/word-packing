/**
 * test_examples.cpp
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <word_packing.hpp>
#include <word_packing/uint_min.hpp>

namespace word_packing::test::examples {

// nb: these aren't really "unit tests", they mainly exist to check whether the examples compile
TEST_SUITE("word_packing_examples") {
    uintmax_t FIB[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181 };

    TEST_CASE("accessor") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        using Pack = uint64_t;
        size_t const bits = 13;
        Pack buffer[word_packing::num_packs_required<Pack>(20, bits)];

        auto fib = word_packing::accessor(buffer, bits);
        fib[0] = 0;
        fib[1] = 1;
        for(int i = 2; i < 20; i++) {
            fib[i] = fib[i-2] + fib[i-1];
            CHECK(fib[i] == FIB[i]);
        }
    }

    TEST_CASE("direct_api_fixed_width") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        using Pack = uint64_t;
        size_t const bits = 13;
        Pack buffer[word_packing::num_packs_required<Pack>(20, bits)];

        auto fib = word_packing::accessor<bits>(buffer);
        fib[0] = 0;
        fib[1] = 1;
        for(int i = 2; i < 20; i++) {
            fib[i] = fib[i-2] + fib[i-1];
            CHECK(fib[i] == FIB[i]);
        }
    }

    TEST_CASE("packed_int_vector") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        word_packing::PackedIntVector fib(100, 13);
        fib[0] = 0;
        fib[1] = 1;
        for(int i = 2; i < 20; i++) {
            fib[i] = fib[i-2] + fib[i-1];
            CHECK(fib[i] == FIB[i]);
        }
        fib.resize(22, 14);
    }

    TEST_CASE("fixed_width_packed_int_vector") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        word_packing::PackedFixedWidthIntVector<13> fib(20);
        fib[0] = 0;
        fib[1] = 1;
        for(int i = 2; i < 20; i++) {
            fib[i] = fib[i-2] + fib[i-1];
            CHECK(fib[i] == FIB[i]);
        }
    }

    TEST_CASE("uint_min") {
        using uint7 =  word_packing::UintMin<7>;  // resolves to uint8_t
        static_assert(std::is_same_v<uint7, uint8_t>);
        using uint12 = word_packing::UintMin<12>; // resolves to uint16_t
        static_assert(std::is_same_v<uint12, uint16_t>);
        using uint32 = word_packing::UintMin<32>; // resolves to uint32_t
        static_assert(std::is_same_v<uint32, uint32_t>);
    }
}

}
