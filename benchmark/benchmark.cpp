#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#include <packed_int_vector.hpp>
#include <packed_fixed_int_vector.hpp>
#include <uint_min.hpp>

// --- BENCHMARK SETUP ---

// the benchmark size
constexpr size_t N = 10'000'000;

// the index type
using Index = uint32_t;
static_assert((size_t)std::numeric_limits<Index>::max() >= N, "Index type too small!");

// the random seed
constexpr size_t SEED = 147;

// ---
uintmax_t VALUES[N];
uintmax_t CHECKSUM;
Index INDICES[N];

class Stopwatch {
private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;

public:
    inline Stopwatch() : start_(Clock::now()) {}
    inline uintmax_t elapsed_time_millis() const { return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start_).count(); }
};

struct GetBenchmarkResult { uintmax_t time, checksum; };

template<typename C>
uintmax_t benchmark_set_sequential(C& container, uintmax_t const* value_sequence) {
    Stopwatch sw;
    for(size_t i = 0; i < N; i++) {
        container[i] = *value_sequence++;
    }
    return sw.elapsed_time_millis();
}

template<typename C>
GetBenchmarkResult benchmark_get_sequential(C const& container) {
    uintmax_t chk = 0;
    Stopwatch sw;
    for(size_t i = 0; i < N; i++) {
        chk += (uintmax_t)container[i];
    }
    return { sw.elapsed_time_millis(), chk };
}

template<typename C>
uintmax_t benchmark_set_random_access(C& container, uintmax_t const* value_sequence, Index const* index_sequence) {
    Stopwatch sw;
    for(size_t i = 0; i < N; i++) {
        auto const j = *index_sequence++;
        auto const x = *value_sequence++;
        container[j] = x;
        assert(container[j] == x);
    }
    return sw.elapsed_time_millis();
}

template<typename C>
GetBenchmarkResult benchmark_get_random_access(C const& container, Index const* index_sequence) {
    uintmax_t chk = 0;
    Stopwatch sw;
    for(size_t i = 0; i < N; i++) {
        auto const j = *index_sequence++;
        assert(container[j] == VALUES[i]);
        chk += (uintmax_t)container[j];
    }
    return { sw.elapsed_time_millis(), chk };
}

struct BenchmarkResult {
    bool chk_seq, chk_rnd;
    uintmax_t time_set_seq, time_set_rnd, time_get_seq, time_get_rnd;

    void print(std::string const& name, size_t bits) {
        std::cout << "RESULT" <<
            " n=" << N <<
            " w=" << bits <<
            " container=" << name <<
            " time_set_seq=" << time_set_seq <<
            " time_get_seq=" << time_get_seq <<
            " chk_seq=" << (chk_seq ? "PASS" : "FAIL") <<
            " time_set_rnd=" << time_set_rnd <<
            " time_get_rnd=" << time_get_rnd <<
            " chk_rnd=" << (chk_rnd ? "PASS" : "FAIL") <<
            std::endl;
    }
};

template<typename C>
BenchmarkResult benchmark_container(C& container) {
    BenchmarkResult r;
    r.time_set_seq = benchmark_set_sequential(container, VALUES);
    auto const get_seq = benchmark_get_sequential(container);
    r.time_get_seq = get_seq.time;
    assert(get_seq.checksum == CHECKSUM);
    r.chk_seq = (get_seq.checksum == CHECKSUM);

    r.time_set_rnd = benchmark_set_random_access(container, VALUES, INDICES);
    auto const get_rnd = benchmark_get_random_access(container, INDICES);
    r.time_get_rnd = get_rnd.time;
    assert(get_rnd.checksum == CHECKSUM);
    r.chk_rnd = (get_rnd.checksum == CHECKSUM);
    return r;
}

void generate_values(size_t bits) {
    uintmax_t const max = UINTMAX_MAX >> (std::numeric_limits<uintmax_t>::digits - bits);
    std::mt19937_64 gen(SEED);
    std::uniform_int_distribution<uintmax_t> dist(0, max);

    CHECKSUM = 0;
    for(size_t i = 0; i < N; i++) {
        auto const x = dist(gen);
        CHECKSUM += x;
        VALUES[i] = x;
    }
}

template<size_t bits>
void run_benchmark() {
    if constexpr(bits == 1)
    {
        std::cout << "# generating first random value sequence ... ";
        std::cout.flush();
        Stopwatch sw;
        generate_values(bits);
        auto const time = sw.elapsed_time_millis();
        std::cout << time << "ms"  << std::endl;
    } else {
        generate_values(bits);
    }
    
    using Uint = pdinklag::UintMin<bits>;

    {
        pdinklag::PackedIntVector<Uint> pvec(N, bits);
        benchmark_container(pvec).print("PackedIntVector", bits);
    }

    {
        pdinklag::PackedFixedIntVector<bits, Uint> pvec(N);
        benchmark_container(pvec).print("PackedFixedIntVector", bits);
    }

    if constexpr(bits == 1)
    {
        std::vector<bool> bv(N);
        benchmark_container(bv).print("std::vector<bool>", bits);
    }

    if constexpr(bits == 8 || bits == 16 || bits == 32 || bits == 64)
    {
        std::vector<Uint> vec(N);
        benchmark_container(vec).print("std::vector", bits);
    }
}

template<size_t bits = 1>
void run_benchmarks() {
    run_benchmark<bits>();
    if constexpr(bits < 64) {
        run_benchmarks<bits+1>();
    }
}

int main(int argc, char** argv) {
    // generate random index sequence
    {
        std::cout << "# generating random access index sequence ... ";
        std::cout.flush();
        Stopwatch sw;
        for(size_t i = 0; i < N; i++) INDICES[i] = i; // iota
        std::mt19937_64 gen(~SEED);
        std::uniform_int_distribution<Index> dist(0, N-1);
        for(size_t i = 0; i < N; i++) std::swap(INDICES[i], INDICES[dist(gen)]); // shuffle
        
        auto const time = sw.elapsed_time_millis();
        std::cout << time << "ms" << std::endl;
    }

    run_benchmarks();
    return 0;
}
