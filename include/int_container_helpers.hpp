#ifndef _INT_CONTAINER_HELPERS_HPP
#define _INT_CONTAINER_HELPERS_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

namespace pdinklag::word_packing_internals {

template<typename IntContainer>
struct IntRef {
    IntContainer* vector;
    size_t index;

    IntRef(IntContainer& _vector, size_t _index) : vector(&_vector), index(_index) {
    }

    IntRef(const IntRef&) = default;
    IntRef(IntRef&&) = default;
    
    IntRef& operator=(const IntRef&) = delete; // ambiguity with value assignment - use explicit casts!
    IntRef& operator=(IntRef&&) = delete; // ambiguity with value assignment - use explicit casts!

    operator uintmax_t() const { return vector->get(index); }
    void operator=(uintmax_t x) { vector->set(index, x); }

    bool operator==(IntRef const&) const = default;
    bool operator!=(IntRef const&) const = default;
};

template<typename IntContainer>
struct ConstIntRef {
    IntContainer const* vector;
    size_t index;

    ConstIntRef(IntContainer const& _vector, size_t _index) : vector(&_vector), index(_index) {
    }

    ConstIntRef(const ConstIntRef&) = default;
    ConstIntRef(ConstIntRef&&) = default;
    ConstIntRef& operator=(const ConstIntRef&) = default;
    ConstIntRef& operator=(ConstIntRef&&) = default;

    operator uintmax_t() const { return vector->get(index); }

    bool operator==(ConstIntRef const&) const = default;
    bool operator!=(ConstIntRef const&) const = default;
};

template<typename IntRef>
struct IntIterator {
    using difference_type = void;
    using value_type = IntRef;
    using pointer = IntRef*;
    using reference = IntRef&;
    using iterator_category = std::bidirectional_iterator_tag;

    IntRef ref;

    IntIterator& operator++()    { ++ref.index; return *this; }
    IntIterator  operator++(int) { auto before = *this; ++(*this); return before; }
    IntIterator& operator--()    { --ref.index; return *this; }
    inline IntIterator  operator--(int) { auto before = *this; --(*this); return before; }

    bool operator==(IntIterator const&) const = default;
    bool operator!=(IntIterator const&) const = default;

    IntRef& operator*() { return ref; }
    IntRef* operator->() { return &ref; }
};

constexpr uintmax_t low_mask(size_t const num) {
    return ~((UINTMAX_MAX << (num - 1)) << 1); // nb: num > 0 is assumed!
}

constexpr size_t idiv_ceil(size_t const a, size_t const b) {
    return ((a + b) - 1ULL) / b;
}

template<std::unsigned_integral PackWord>
constexpr size_t pack_word_count(size_t const num, size_t const width) {
    return idiv_ceil(num * width, std::numeric_limits<PackWord>::digits);
}

template<std::unsigned_integral PackWord>
auto allocate_pack_words(size_t const capacity, size_t const width) {
    return std::make_unique<PackWord[]>(pack_word_count<PackWord>(capacity, width));
}

}

#endif
