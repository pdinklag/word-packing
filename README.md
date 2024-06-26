# Word-Packed Integer Vectors for C++

This header-only C++20 library provides implementations for vectors of packed (unsigned) integers, commonly referred to as *word packing* in the literature. These integers are represented by an arbitrary number of bits (called *width*), which allows for saving space when they are known to be limited to a universe with a worst-case entropy other than the natively supported 8, 16, 32 or 64 bits.

Naturally, due to the fact that words are no longer aligned to hardware word sizes, access to packed vectors is slower than aligned access. To achieve access as fast as possible, depending on your use case, we recommend one of the corresponding implementations:

| Word width known at &hellip; | Dynamic resizing required | Recommended Implementation                                   |
| ---------------------------- | ------------------------- | ------------------------------------------------------------ |
| Compile time                 | yes                       | [PackedFixedWidthIntVector](#packedfixedwidthintvector) / [BitVector](#bitvector) |
| Compile time                 | no                        | [Accessors (Fixed Width)](#fixed-width-accessors) / [Bit Accessors](#bit-accessors) |
| Runtime                      | yes                       | [PackedIntVector](#packedintvector)                          |
| Runtime                      | no                        | [Accessors](#accessors)                                      |

The fixed-width implementations have specializations for accessing words of width 1 (so-called "bits"), which is particularly faster than for greater widths.

### Requirements

This library is written in C++20, a corresponding compiler is required that fully supports concepts. Tests have been done only with GCC 11. Apart from that, the library as no external dependencies. For building the [benchmark](#benchmark) or the [unit tests](#unit-tests), CMake is required.

### License

```
MIT License

Copyright (c) Patrick Dinklage

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Unit Tests

The library comes with unit tests powered by [doctest](https://github.com/doctest/doctest).

Using CMake, you can build and run the benchmark using the following chain of commands in the repository root:

```sh
mkdir build; cd build
cmake ..
make
make test
```

## Usage

The library is header only, so all you need to do is make sure it's in your include path.

In case you use CMake, you can embed this repository into yours (e.g., as a git submodule) and add it like so:

```cmake
add_subdirectory(path/to/word-packing)
```

You can then link against the `word-packing` interface library, which will automatically add the include directory to your target.

### Accessors

The accessors allow treating any buffer of unsigned integers as a container of packed words. Their usage is straightforward and they can be used as garbage objects that will likely be optimized away completely by any modern compiler.

Consider the following example:

```cpp
#include <word_packing.hpp>
// ...

// we compute the first 20 Fibonacci numbers, which fit into 13 bits each
using Pack = uint64_t;
size_t const bits = 13;
Pack buffer[word_packing::num_packs_required<Pack>(20, bits)];

auto fib = word_packing::accessor(buffer, bits);
fib[0] = 0;
fib[1] = 1;
for(int i = 2; i < 20; i++) {
    fib[i] = fib[i-2] + fib[i-1];
}
```

In the example above, you will also notice the utility function `num_packs_required`, which computes the number of packs of the given type required to store a given number of words with the given bit width.

#### Fixed Width Accessors

If the width per integer is already known at compile time, it is recommended to use the overloads that make use of this fact by providing better compile-time optimization opportunities.

```cpp
#include <word_packing.hpp>
// ...

// we compute the first 20 Fibonacci numbers, which fit into 13 bits each
using Pack = uint64_t;
size_t const bits = 13;
Pack buffer[word_packing::num_packs_required<Pack>(20, bits)];

auto fib = word_packing::accessor<bits>(buffer);
fib[0] = 0;
fib[1] = 1;
for(int i = 2; i < 20; i++) {
    fib[i] = fib[i-2] + fib[i-1];
}
```

#### Bit Accessors

In case you wish to access individual bits, you can use the convenience function `word_packing::bit_accessor` to retrieve a specialized and faster accessor. Note that this is equivalent to calling `word_packing::accessor<1>`.

### Containers

This library also provides containers that are mostly STL compatible; they can be used very much like `std::vector` and also use capacity doubling when growing. The most notable exception is that if you construct a packed integer vector with a given size or resize it, the allocated memory is *not* initialized.

Both PackedFixedWidthIntVector and PackedIntVector support integer widths between 1 and the width of your widest available integer (`uintmax_t`), each including. That said, by using a width of 1, the vectors act like bit vectors similar `std::vector<bool>`. However, they have not been optimized for this case and a dedicated bit vector implementation is likely a better choice.

When assigning integers, only the low bits corresponding container's integer width are actually stored. Let's say the container's integer width is 7, and you write the value 255 to some entry (which requires 8 bits), the actually stored integer is 127, corresponding to only the lowest 7 bits.

Both vectors offer to specify the unsigned integer type used to pack integers into. For example, by choosing `uint16_t` as the pack type, values are packed into 16-bit integers. The width of a word pack must be at least the width of an integer in the container. By default, integers are packed into the widest natively supported integer type (`uintmax_t`).

#### PackedIntVector

Use this class if the integer width is known already at compile time. It needs to be passed to the constructor.

```cpp
#include <word_packing.hpp>
// ...

// we compute the first 20 Fibonacci numbers, which fit into 13 bits each
word_packing::PackedIntVector fib(100, 13);
fib[0] = 0;
fib[1] = 1;
for(int i = 2; i < 20; i++) {
    fib[i] = fib[i-2] + fib[i-1];
}
```

It is possible to change the integer width at runtime using a special overload of `resize`:

```cpp
fib.resize(22, 14); // after this operation, each integer has width 14 bits
```

#### PackedFixedWidthIntVector

Use this class if the integer width is known already at compile time. It is passed as a template parameter.

```cpp
#include <word_packing.hpp>
// ...

// we compute the first 20 Fibonacci numbers, which fit into 13 bits each
word_packing::PackedFixedWidthIntVector<13> fib(20);
fib[0] = 0;
fib[1] = 1;
for(int i = 2; i < 20; i++) {
    fib[i] = fib[i-2] + fib[i-1];
}
```

#### BitVector

For accessing single bits, the alias type `word_packing::BitVector` provides a specialized and faster implemenation. Note that the same is achieved if you use `word_packing::PackedFixedIntVector<1>`.

### UintMin

Sometimes, it is desirable to work with the smallest natively supported integer type that fits a certain number of bits. An example would be to use it as the pack type and minimize waste. This can be selected using `std::conditional`. This library provides a convenience type used like so:

```cpp
#include <word_packing/uint_min.hpp>
// ...
using uint7 =  word_packing::UintMin<7>;  // resolves to uint8_t
using uint12 = word_packing::UintMin<12>; // resolves to uint16_t
using uint32 = word_packing::UintMin<32>; // resolves to uint32_t
// ...
```

## Benchmark

The library comes with a benchmark that tests the performance of the provided implementations and compares them against the STL counterparts of the packed vectors, `std::vector` (size known at runtime) and `std::array` (size known at compile time).

The benchmark allocates a container for *N* (currently fixed to 100 million) integers and conducts the following operations:

1. Set all integers in *N* to random numbers in sequential order (*set_seq*)
2. Read all integers from *N* in sequential order (*get_seq*)
3. Set all integers in *N* to random numbers  in random order (*set_rnd*)
4. Read all integers from *N* in random order (*get_rnd*)

All random numbers are pregenerated and not included in the measurement; the only measured overhead comes from reading those pregenerated sequences in sequential order, as well as from summing up the read integers to compute a checksum.

Note that on a 64-bit machine, **the benchmark requires 20N bytes &approx; 1.9 GiB of RAM**.

### Results

We have performed the benchmark on different systems with the following processors:

1. Intel Xeon E5-2640 v4 ([view plots](figures/benchmark-results/xeon.pdf)) ([view raw results](figures/benchmark-results/xeon.txt))
2. Intel Core i7-8700 ([view plots](figures/benchmark-results/i7.pdf)) ([view raw results](figures/benchmark-results/i7.txt))
3. AMD EPYC 7452 ([view plots](figures/benchmark-results/epyc.pdf)) ([view raw results](figures/benchmark-results/epyc.txt))

Please use the links to view the raw results (output of three iterations concatenated) or some precompiled plots (showing the average over all 3 iterations). In the plots, we compute the read / write throughputs for selected integer widths; for other widths, the results are very similar.

We observe roughly the following (this is heavily summarized):

* In the cases where direct comparisons to `std::vector` are possible (bit widths 1, 8, 16, 32 and 64), the *PackedFixedWidthIntVector* is only marginally slower than `std::vector`, while the *PackedIntVector* is notably slower (up to a factor of 3).
* In aligned cases other than the above, where the integer width divides 64 (bit widths 2 and 4), the *PackedFixedWidthIntVector* is at least 50% faster than *PackedIntVector*.
* In all other cases, *PackedFixedWidthIntVector* achieves roughly a 5% speedup over *PackedIntVector*.

### Building & Running

Using CMake, you can build and run the benchmark using the following chain of commands in the repository root:

```sh
mkdir build; cd build
cmake ..
make benchmark
benchmark/benchmark # run it
```

### Output

The output consists of lines containing pairs of keys and values as described in the following table.

| Key            | Value Description                                            |
| -------------- | ------------------------------------------------------------ |
| `n`            | The benchmark size *N*.                                      |
| `w`            | The width per integer in the container.                      |
| `container`    | The container for which the benchmark was conducted.         |
| `time_set_seq` | The time, in milliseconds, it took to set the *N* integers in sequential order. |
| `time_get_seq` | The time, in milliseconds, it took to read the *N* integers in sequential order. |
| `chk_seq`      | `PASS` if the sum of read values after writing them was correct, `FAIL` otherwise. |
| `time_set_rnd` | The time, in milliseconds, it took to set the *N* integers in random order. |
| `time_get_rnd` | The time, in milliseconds, it took to read the *N* integers in random order. |
| `chk_rnd`      | `PASS` if the sum of read values after writing them was correct, `FAIL` otherwise. |

## Implementation

The core implementation of reading and writing packed containers is found in the internal `get` and `set` overloads in `word_packing/internal/impl.hpp` and consists of all the bit twiddling you would imagine. Generally, regarding word packing, consider the following figure:

![figures/layout.png](figures/layout.png)

We have an array *A* of integers of width *w* packed into an array *A'* of packs. When accessing the *i*-th integer, it there are two cases to consider: (1) it is completely contained in the *a*-th pack, or (2) its low bits are contained in the *a*-th pack and its high bits are contained in the *b*-th pack. We compute the number of bits *wa* and *wb* contained in *a* or *b*, respectively (*wa + wb = w* holds), as well as the number *da* of low bits from *a* that we can ignore. Using binary arithmetics, we can use this to read or write the desired integer.

The implementation of this procedure is straightforward. There are two pairs of get and set function implementations: `get_rt / set_rt` are for the case where the width per integer is known only at runtime, whereas `get_ct / set_ct` received the width as a compile-time parameter. In the latter, we further optimize the code for the aligned case, that is, if the integer width divides the width of a pack.

### Bit Access

In the special case that *w = 1*, we are guaranteed that *a = b* and *wa = wb = 1*, which heavily simplifies the implementation, resulting in increased access performance. The `get_ct / set_ct` functions provide specializations for this case that are selected at compile time with no runtime overhead.

Note that the runtime implementations `get_rt / set_rt` do *not* feature this specialization, because this would induce an additional branch (to test whether *w == 1*) to *every* access and may slow down access times by a factor.