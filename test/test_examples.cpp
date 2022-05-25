/**
 * test_examples.cpp
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
#include <word_packing.hpp>
#include <uint_min.hpp>

namespace word_packing::test::examples {

// nb: these aren't really "unit tests", they mainly exist to check whether the examples compile
TEST_SUITE("word_packing_examples") {
    TEST_CASE("direct_api") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        using Pack = uint64_t;
        size_t const bits = 13;
        auto const mask = word_packing::low_mask(bits); // precompute the bit mask to improve performance

        Pack fib[word_packing::num_packs_required<Pack>(20, bits)];
        word_packing::set(fib, 0, 0, bits, mask); // fib(1) = 0
        word_packing::set(fib, 1, 1, bits, mask); // fib(2) = 1
        for(int i = 2; i < 20; i++) {
            auto fib_i2 = word_packing::get(fib, i-2, bits, mask);
            auto fib_i1 = word_packing::get(fib, i-1, bits, mask);
            word_packing::set(fib, i, fib_i2 + fib_i1, bits, mask); // fib(i) = fib(i-2) + fib(i-1)
        }
    }

    TEST_CASE("direct_api_fixed_width") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        using Pack = uint64_t;
        constexpr size_t bits = 13;

        Pack fib[word_packing::num_packs_required<Pack>(20, bits)];
        word_packing::set<bits>(fib, 0, 0); // fib(1) = 0
        word_packing::set<bits>(fib, 1, 1); // fib(2) = 1
        for(int i = 2; i < 20; i++) {
            auto fib_i2 = word_packing::get<bits>(fib, i-2);
            auto fib_i1 = word_packing::get<bits>(fib, i-1);
            word_packing::set<bits>(fib, i, fib_i2 + fib_i1); // fib(i) = fib(i-2) + fib(i-1)
        }
    }

    TEST_CASE("packed_int_vector") {
        // we compute the first 20 Fibonacci numbers, which fit into 13 bits each
        word_packing::PackedIntVector fib(100, 13);
        fib[0] = 0;
        fib[1] = 1;
        for(int i = 2; i < 20; i++) {
            fib[i] = fib[i-2] + fib[i-1];
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
