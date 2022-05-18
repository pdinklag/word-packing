#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <packed_int_vector.hpp>

namespace pdinklag::test::packed_int_vector {

using PackedIntVector = pdinklag::PackedIntVector<uint64_t>;
constexpr size_t MAX_WIDTH = 63;

#include "test_packed_int_vector_impl.hpp"

}