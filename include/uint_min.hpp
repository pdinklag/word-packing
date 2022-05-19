#ifndef _UINT_MIN_HPP
#define _UINT_MIN_HPP

#include <cstdint>
#include <type_traits>

namespace pdinklag {

/**
 * \brief The minimum unsigned integer type that consists of at least the specified number of bits
 * 
 * This only supports up to 64 bits.
 * 
 * \tparam bits the number of bits that the integer type must be able to fit
 */
template<unsigned bits>
using UintMin =
    typename std::conditional<(bits > 64), void,
        typename std::conditional<(bits > 32), uint64_t,
            typename std::conditional<(bits > 16), uint32_t,
                typename std::conditional<(bits > 8), uint16_t, uint8_t>::type
            >::type
        >::type
    >::type;

}

#endif
