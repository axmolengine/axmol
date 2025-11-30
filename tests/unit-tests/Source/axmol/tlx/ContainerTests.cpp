#include <doctest.h>
#include "TestUtils.h"
#include <iostream>

// #include <flat_set>
// #include <flat_map>

#include <set>
#include <unordered_set>

#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/flat_set.hpp"
#include "axmol/tlx/flat_map.hpp"

namespace tlx
{
// flat_set traits
template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
struct flat_set_traits1
{
    using key_type       = Key;
    using value_type     = Key;
    using key_compare    = Compare;
    using value_compare  = Compare;
    using allocator_type = Alloc;
    using container_type = tlx::vector<value_type, Alloc>;

    static constexpr bool allow_duplicates = false;
};

/// flat_set
template <class Key, class Compare = std::less<Key>, class Alloc = std::allocator<Key>>
class flat_set1 : public detail::flat_set_base<flat_set_traits1<Key, Compare, Alloc>>
{
    using impl_type = detail::flat_set_base<flat_set_traits1<Key, Compare, Alloc>>;

public:
    using impl_type::impl_type;
};
}  // namespace tlx

template <typename _Cont1, typename _Cont2>
static constexpr bool vector_equals(const _Cont1& c1, const _Cont2& c2)
{
    return c1.size() == c2.size() && 0 == memcmp(c1.data(), c2.data(), c1.size() * sizeof(typename _Cont1::value_type));
}

template <typename Set>
static auto benchmark_set(const std::string& name, const std::vector<int>& keys)
{
    Set s;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto k : keys)
    {
        s.emplace(k);
    }

    auto mid1 = std::chrono::high_resolution_clock::now();

    volatile int sum = 0;
    for (auto k : keys)
    {
        auto it = s.find(k);
        if (it != s.end())
            sum += *it;
    }

    auto mid2 = std::chrono::high_resolution_clock::now();

    for (const auto& v : s)
    {
        sum += v;
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto insert_time = std::chrono::duration<double, std::milli>(mid1 - start).count();
    auto find_time   = std::chrono::duration<double, std::milli>(mid2 - mid1).count();
    auto iter_time   = std::chrono::duration<double, std::milli>(end - mid2).count();

    std::cout << name << " results:\n";
    std::cout << "  Insert: " << insert_time << " ms\n";
    std::cout << "  Find:   " << find_time << " ms\n";
    std::cout << "  Iterate:" << iter_time << " ms\n";
    std::cout << "  Sum:    " << sum << "\n\n";

    return s;
}

template <typename Map>
static auto benchmark_map(const std::string& name, const std::vector<int>& keys)
{
    Map m;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto k : keys)
    {
        m.emplace(k, k * 2);
    }

    auto mid1 = std::chrono::high_resolution_clock::now();

    volatile int sum = 0;
    for (auto k : keys)
    {
        auto it = m.find(k);
        if (it != m.end())
            sum += it->second;
    }

    auto mid2 = std::chrono::high_resolution_clock::now();

    for (const auto& kv : m)
    {
        sum += kv.second;
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto insert_time = std::chrono::duration<double, std::milli>(mid1 - start).count();
    auto find_time   = std::chrono::duration<double, std::milli>(mid2 - mid1).count();
    auto iter_time   = std::chrono::duration<double, std::milli>(end - mid2).count();

    std::cout << name << " results:\n";
    std::cout << "  Insert: " << insert_time << " ms\n";
    std::cout << "  Find:   " << find_time << " ms\n";
    std::cout << "  Iterate:" << iter_time << " ms\n";
    std::cout << "  Sum:    " << sum << "\n\n";

    return m;
}

static void run_benchmark()
{
    constexpr size_t N = 100000;
    std::vector<int> keys(N);

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(1, N * 10);

    for (size_t i = 0; i < N; ++i)
    {
        keys[i] = dist(rng);
    }

    auto s1 = benchmark_set<std::set<int>>("std::set", keys);
    auto s2 = benchmark_set<std::unordered_set<int>>("std::unordered_set", keys);
    auto s3 = benchmark_set<tlx::hash_set<int>>("tlx::hash_set", keys);
    //auto s4 = benchmark_set<std::flat_set<int>>("std::flat_set", keys);
    auto s5 = benchmark_set<tlx::flat_set<int>>("tlx::flat_set", keys);
    auto s6 = benchmark_set<tlx::flat_set1<int>>("tlx::flat_set1", keys);

    auto m1 = benchmark_map<std::map<int, int>>("std::map", keys);
    auto m2 = benchmark_map<std::unordered_map<int, int>>("std::unordered_map", keys);
    auto m3 = benchmark_map<tlx::hash_map<int, int>>("tlx::hash_map", keys);
    //auto m4 = benchmark_map<std::flat_map<int, int>>("std::flat_map", keys);
    auto m5 = benchmark_map<tlx::flat_map<int, int>>("tlx::flat_map", keys);

    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

    CHECK(vector_equals(keys, s5.keys()));
    CHECK(vector_equals(keys, s6.keys()));
    // CHECK(keys == m5.keys());
}

TEST_SUITE("tlx/Containers")
{
    // !!!Don't invoke FileUtils::getInstacne at here, it's dangerous due to
    // The test suite function will invoke before entrypoint `main`, it will cause
    // crash on Linux(maybe others), crt not initalized properly yet.
#define fu FileUtils::getInstance()

    TEST_CASE("BenchmarkTest")
    {
        run_benchmark();
    }
}
