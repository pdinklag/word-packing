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
    IntIterator operator--(int)  { auto before = *this; --(*this); return before; }

    bool operator==(IntIterator const&) const = default;
    bool operator!=(IntIterator const&) const = default;

    IntRef& operator*() { return ref; }
    IntRef* operator->() { return &ref; }
};

template<typename Impl>
class IntContainer {
private:
    Impl* const impl;

    using ImplIntRef = IntRef<Impl>;
    using ImplConstIntRef = ConstIntRef<Impl>;

protected:
    IntContainer() : impl((Impl*)this) {}
    IntContainer(IntContainer&&) : IntContainer() {}
    IntContainer(IntContainer const&) : IntContainer() {}
    IntContainer& operator=(IntContainer&&) { return *this; }
    IntContainer& operator=(IntContainer const&) { return *this; }

public:
    /**
     * \brief Retrieves a specific integer from the vector
     * 
     * This function simply forwards to \ref get .
     * 
     * \param i the index of the integer
     * \return the integer at the given index
     */
    uintmax_t operator[](size_t i) const { return impl->get(i); }

    /**
     * \brief Provides read/write access to a specific integer in the vector
     * 
     * \param i the index of the integer
     * \return a proxy allowing reading and writing
     */
    auto operator[](size_t i) { return ImplIntRef(*impl, i); }

    /**
     * \brief Returns an iterator to the first integer
     * 
     * \return an iterator to the first integer 
     */
    auto begin() { return IntIterator<ImplIntRef> { ImplIntRef(*impl, 0) }; }

    /**
     * \brief Returns an iterator to right beyond the last integer
     * 
     * \return an iterator to right beyond the last integer
     */
    auto end() { return IntIterator<ImplIntRef> { ImplIntRef(*impl, impl->size()) }; }

    /**
     * \brief Returns an iterator to the i-th integer
     * 
     * \param i the number of the integer to provide and iterator to
     * \return a const iterator to the i-th integer 
     */
    auto at(size_t i) { return IntIterator<ImplConstIntRef> { ImplIntRef(*impl, i) }; }

    /**
     * \brief Returns a const iterator to the first integer
     * 
     * \return a const iterator to the first integer 
     */
    auto begin() const { return IntIterator<ImplConstIntRef> { ImplConstIntRef(*impl, 0) }; }

    /**
     * \brief Returns a const iterator to right beyond the last integer
     * 
     * \return a const iterator to right beyond the last integer
     */
    auto end() const { return IntIterator<ImplConstIntRef> { ImplConstIntRef(*impl, impl->size()) }; }

    /**
     * \brief Returns a const iterator to the i-th integer
     * 
     * \param i the number of the integer to provide and iterator to
     * \return a const iterator to the i-th integer
     */
    auto at(size_t i) const { return IntIterator<ImplConstIntRef> { ImplConstIntRef(*impl, i) }; }

    /**
     * \brief Provides read/write access to the first integer
     * 
     * \return a proxy allowing reading and writing
     */
    auto front() { return ImplIntRef(*impl, 0); }

    /**
     * \brief Returns the first integer
     * 
     * \return the first integer
     */
    uintmax_t front() const { return impl->get(0); }

    /**
     * \brief Provides read/write access to the last integer
     * 
     * \return a proxy allowing reading and writing
     */
    auto back() { return ImplIntRef(*impl, impl->size() - 1); }

    /**
     * \brief Returns the last integer
     * 
     * \return the last integer
     */
    uintmax_t back() const { return impl->get(impl->size() - 1); }

    /**
     * \brief Tests whether the vector is empty
     * 
     * \return true if the size equals zero
     * \return false otherwise
     */
    bool empty() const { return impl->size() == 0; }
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
