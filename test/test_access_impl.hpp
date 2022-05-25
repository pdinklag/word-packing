/**
 * test_access_impl.hpp
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

TEST_SUITE("packed_int_access") {
    TEST_CASE("set and get") {
        auto iota_test = [](size_t width){
            size_t const num = 9'999;
            auto const mask = word_packing::internal::low_mask(width);
            uintmax_t const off = (1ULL << width) - num;

            Pack packs[num_packs_required<Pack>(num, width)];
            for(size_t i = 0; i < num; i++) {
                word_packing::internal::set(packs, i, off + i, width, mask);
            }

            for(size_t i = 0; i < num; i++) {
                auto const expect = (off + i) & mask;
                CHECK(word_packing::internal::get(packs, i, width, mask) == expect);
            }
        };

        for(size_t w = 1; w <= MAX_WIDTH; w++) iota_test(w);
    }
}
