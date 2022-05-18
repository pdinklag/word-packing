#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <packed_int_vector.hpp>

namespace pdinklag::test::packed_int_vector {

using PackedIntVector = pdinklag::PackedIntVector<uint64_t>;

TEST_SUITE("packed_int_vector") {
    TEST_CASE("set and get") {
        auto iota_test = [](size_t width){
            size_t const num = 9'999;
            auto const mask = low_mask(width);
            uintmax_t const off = (1ULL << width) - num;

            PackedIntVector v(num, width);
            
            CHECK(v.size() == num);
            CHECK(v.capacity() == num);
            CHECK(v.width() == width);

            for(size_t i = 0; i < num; i++) {
                v[i] = off + i;
            }

            for(size_t i = 0; i < num; i++) {
                auto const expect = (off + i) & mask;
                CHECK(v[i] == expect);
            }
        };

        for(size_t w = 1; w < 64; w++) iota_test(w);
    }

    TEST_CASE("resize") {
        auto resize_test = [](size_t width){
            size_t const num = 3'333;
            auto const mask = low_mask(width);

            PackedIntVector v(num, width);
            CHECK(v.size() == num);
            CHECK(v.capacity() == num);
            CHECK(v.width() == width);

            for(size_t i = 0; i < num; i++) {
                v[i] = i;
            }

            // shrink to a smaller size
            size_t const smaller = 1'234;
            v.resize(smaller);

            CHECK(v.size() == smaller);
            CHECK(v.capacity() == num);
            CHECK(v.width() == width);
            for(size_t i = 0; i < smaller; i++) {
                auto const expect = i & mask;
                CHECK(v[i] == expect);
            }

            // grow back to the old size
            v.resize(num);
            CHECK(v.size() == num);
            CHECK(v.capacity() == num);
            CHECK(v.width() == width);

            for(size_t i = smaller; i < num; i++) {
                v[i] = i;
            }

            for(size_t i = 0; i < num; i++) {
                auto const expect = i & mask;
                CHECK(v[i] == expect);
            }
        };

        for(size_t w = 1; w < 64; w++) resize_test(w);
    }

    TEST_CASE("empty") {
        auto empty_test = [](size_t width){
            PackedIntVector v(0, width);
            CHECK(v.size() == 0);
            CHECK(v.empty());
            CHECK(v.begin() == v.end());
        };
        for(size_t w = 1; w < 64; w++) empty_test(w);
    }

    TEST_CASE("reserve") {
        auto reserve_test = [](size_t width){
            PackedIntVector v(0, width);
            CHECK(v.capacity() == 0);
            CHECK(v.empty());
            
            // reserve some
            size_t const cap = 100;
            v.reserve(cap);
            CHECK(v.capacity() == cap);
            CHECK(v.empty());

            // reserve less
            size_t const less = 99;
            v.reserve(less);
            CHECK(v.capacity() == cap);
            CHECK(v.empty());

            // reserve more
            size_t const more = 101;
            v.reserve(more);
            CHECK(v.capacity() == more);
            CHECK(v.empty());
        };

        for(size_t w = 1; w < 64; w++) reserve_test(w);
    }

    TEST_CASE("append") {
        auto append_test = [](size_t width){
            PackedIntVector v(0, width);

            size_t const num = 128;
            auto const mask = low_mask(width);

            for(size_t i = 0; i < num; i++) {
                v.push_back(i);
                CHECK(v.size() == i + 1);
                CHECK(v.capacity() == std::bit_ceil(i + 1));
                CHECK(v[i] == (i & mask));
            }

            for(size_t i = 0; i < num; i++) {
                CHECK(v[i] == (i & mask));
            }
        };

        for(size_t w = 1; w < 64; w++) append_test(w);
    }

    TEST_CASE("pop_back") {
        auto pop_test = [](size_t width){
            size_t const num = 128;
            auto const mask = low_mask(width);

            PackedIntVector v(num, width);
            for(size_t i = 0; i < num; i++) {
                v[i] = i;
            }

            for(size_t i = 0; i < num; i++) {
                size_t const iinv = num - 1 - i;
                CHECK(v.back() == (iinv & mask));
                CHECK(v.size() == num - i);
                CHECK(v.capacity() == num);
                v.pop_back();
            }

            CHECK(v.empty());
            CHECK(v.capacity() == num);
        };

        for(size_t w = 1; w < 64; w++) pop_test(w);
    }

    TEST_CASE("shrink_to_fit") {
        auto shrink_test = [](size_t width){
            size_t const num = 128;

            PackedIntVector v(num, width);
            v.shrink_to_fit();
            CHECK(v.size() == num);
            CHECK(v.capacity() == num);

            size_t const less = 47;
            v.resize(less);
            v.shrink_to_fit();
            CHECK(v.size() == less);
            CHECK(v.capacity() == less);

            v.push_back(0);
            CHECK(v.size() == less + 1);
            CHECK(v.capacity() == 2 * less);

            v.shrink_to_fit();
            CHECK(v.size() == less + 1);
            CHECK(v.capacity() == less+1);
        };

        for(size_t w = 1; w < 64; w++) shrink_test(w);
    }

    TEST_CASE("clear") {
        auto clear_test = [](size_t width){
            size_t const num = 128;

            PackedIntVector v(num, width);
            v.clear();
            CHECK(v.empty());
            CHECK(v.capacity() == num);

            v.shrink_to_fit();
            CHECK(v.empty());
            CHECK(v.capacity() == 0);
        };

        for(size_t w = 1; w < 64; w++) clear_test(w);
    }

    TEST_CASE("iterator") {
        auto iterator_test = [](size_t width) {
            size_t const num = 3'333;
            auto const mask = low_mask(width);

            PackedIntVector v(num, width);
            uintmax_t sum = 0;
            for(size_t i = 0; i < num; i++) {
                v[i] = i;
                sum += (i & mask);
            }

            size_t count = 0;
            uintmax_t chk = 0;
            for(auto x : v) {
                chk += x;
                ++count;
            }

            CHECK(count == num);
            CHECK(chk == sum);
        };

        for(size_t w = 1; w < 64; w++) iterator_test(w);
    }

    TEST_CASE("const_iterator") {
        auto const_iterator_test = [](size_t width) {
            size_t const num = 3'333;
            auto const mask = low_mask(width);

            PackedIntVector v(num, width);
            uintmax_t sum = 0;
            for(size_t i = 0; i < num; i++) {
                v[i] = i;
                sum += (i & mask);
            }

            PackedIntVector const& cv = v;
            size_t count = 0;
            uintmax_t chk = 0;
            for(auto x : cv) {
                chk += x;
                ++count;
            }

            CHECK(count == num);
            CHECK(chk == sum);
        };

        for(size_t w = 1; w < 64; w++) const_iterator_test(w);
    }
}

}