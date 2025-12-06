#include <doctest.h>
#include "TestUtils.h"
#include <iostream>

#if __has_include(<flat_map>)
#    include <flat_map>
#    define _AX_STL_HAS_FLAT_MAP 1
#else
#    define _AX_STL_HAS_FLAT_MAP 0
#endif

#if __has_include(<flat_set>)
#    include <flat_set>
#    define _AX_STL_HAS_FLAT_SET 1
#else
#    define _AX_STL_HAS_FLAT_SET 0
#endif

#include <set>
#include <unordered_set>

#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/flat_set.hpp"
#include "axmol/tlx/flat_map.hpp"

#include "axmol/rhi/IndexArray.h"

#if defined(_WIN32) || defined(_WIN64)

#    define __TRY     if (true)
#    define __CATCH   else
#    define __FINALLY if (true)
#    define __ENDTRY

#else  // POSIX / Unix-like

#    include <signal.h>
#    include <setjmp.h>

static thread_local sigjmp_buf __doctest_jumpbuf;

void __doctest_signal_handler(int sig)
{
    AXLOGI("Caught signal: {}", sig);
    ::siglongjmp(__doctest_jumpbuf, 1);  // jump back to safe point
}

#    define __TRY                                        \
        do                                               \
        {                                                \
            struct sigaction sa{};                       \
            sa.sa_handler = __doctest_signal_handler;    \
            sigemptyset(&sa.sa_mask);                    \
            sa.sa_flags = 0;                             \
            for (int i = 1; i < NSIG; ++i)               \
            {                                            \
                if (i == SIGKILL || i == SIGSTOP)        \
                    continue;                            \
                sigaction(i, &sa, nullptr);              \
            }                                            \
            int __ret = sigsetjmp(__doctest_jumpbuf, 1); \
            if (__ret == 0)

#    define __CATCH else

#    define __FINALLY                               \
        {                                           \
            struct sigaction sa_default{};          \
            sa_default.sa_handler = SIG_DFL;        \
            sigemptyset(&sa_default.sa_mask);       \
            sa_default.sa_flags = 0;                \
            for (int i = 1; i < NSIG; ++i)          \
            {                                       \
                if (i == SIGKILL || i == SIGSTOP)   \
                    continue;                       \
                sigaction(i, &sa_default, nullptr); \
            }                                       \
        }

#    define __ENDTRY \
        }            \
        while (0)

#endif

template <typename _Tp>
using my_flat_set = _TLX flat_set<_Tp, std::less<_Tp>, tlx::vector<_Tp>>;

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

    auto s5 = benchmark_set<tlx::flat_set<int>>("tlx::flat_set", keys);
    auto s6 = benchmark_set<my_flat_set<int>>("my_flat_set", keys);
#if _AX_STL_HAS_FLAT_SET
    auto s4 = benchmark_set<std::flat_set<int>>("std::flat_set", keys);
#endif

    auto m1 = benchmark_map<std::map<int, int>>("std::map", keys);
    auto m2 = benchmark_map<std::unordered_map<int, int>>("std::unordered_map", keys);
    auto m3 = benchmark_map<tlx::hash_map<int, int>>("tlx::hash_map", keys);
    auto m5 = benchmark_map<tlx::flat_map<int, int>>("tlx::flat_map", keys);
#if _AX_STL_HAS_FLAT_MAP
    auto m4 = benchmark_map<std::flat_map<int, int>>("std::flat_map", keys);
#endif

    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

    CHECK(keys == s5.keys());
    CHECK(keys == s6.keys());
}

TEST_SUITE("tlx/Containers")
{
#define fu FileUtils::getInstance()

    TEST_CASE("VectorTest")
    {
        static int _dtor_invoke_counter = 0;

        struct NonTrivalCtor1
        {
            int value = 123;
        };

        struct TrivalCtor1
        {
            int value;
        };

        struct NonTrivialDtor1
        {
            ~NonTrivialDtor1()
            {
                ++_dtor_invoke_counter;
                if (ptr)
                    free(ptr);
            }
            void* ptr;
        };

        ax::setLogFmtFlag(ax::LogFmtFlag::Level | ax::LogFmtFlag::TimeStamp);

        std::list<char> list0{'2', '3', '4', '5', 'b'};
        tlx::vector<char> arr0;
        arr0.reserve(list0.size());
        for (auto ch : list0)
            arr0.emplace_back(ch);
        CHECK(arr0 == list0);

        tlx::vector<char> arr1;
        std::string msg = "aaaaaaaaaaaaaafbbbbbbbbbbbbbbbbbcccccccccdfefffffff";
        arr1 += msg;
        CHECK(arr1 == msg);

        tlx::vector<NonTrivalCtor1> arr2;
        arr2.resize(2);
        CHECK((arr2[0].value == 123 && arr2[1].value == 123 && arr2.size() == 2));

        // normal vector whti trival ctor types, shoud initialized to zero
        tlx::vector<TrivalCtor1> arr3;
        arr3.resize(2);
        CHECK((arr3[0].value == 0 && arr3[1].value == 0));

        // while, if you run unittest with Xcode debugger, the debugger still raise exception
        __TRY
        {
            // pod vector, no auto fill when dtor is trivial
            tlx::pod_vector<NonTrivalCtor1> arr4;
            arr4.resize(2);
            CHECK((arr4[0].value != 123 && arr4[1].value != 123 && arr4.size() == 2));

            tlx::pod_vector<TrivalCtor1> arr5;
            arr5.resize(2);

#ifndef __APPLE__
            CHECK((arr5[0].value != 0 && arr5[1].value != 0));
#endif
            // we can safe access initialized member without exception catch
            arr5.resize(4, TrivalCtor1{39});
            CHECK((arr5[2].value == 39 && arr5[3].value == 39));

            arr5.resize(128, TrivalCtor1{66});
            CHECK((arr5[2].value == 39 && arr5[3].value == 39));

            CHECK((arr5[10].value == 66 && arr5[22].value == 66));

            AXLOGI("Access uninitialzed object membmer done (non optimized build or non-Apple platforms)");
        }
        __CATCH
        {
            AXLOGI("Access uninitialzed object member raise SIGILL(4)/SIGTRAP(5) (optimized build on Apple platforms)");
        }
        __FINALLY
        {
            ;
        }
        __ENDTRY;

        // shoud report compile error, non trivial dtors types can't use tlx::pod_vector
        // tlx::pod_vector<NonTrivialDtor1> arr6;

        {
            _dtor_invoke_counter = 0;
            tlx::vector<NonTrivialDtor1> arr7;

            arr7.resize(9);
        }

        CHECK(_dtor_invoke_counter == 9);
    }

    TEST_CASE("FlatSetTest")
    {
        tlx::flat_set<int> set1;
        set1.emplace(25);
        set1.emplace(10);
        set1.emplace(34);
        set1.emplace(65);
        set1.emplace(35);
        set1.emplace(10);

        CHECK(set1.size() == 5);
        set1.erase(10);
        CHECK(set1.size() == 4);

        tlx::flat_multiset<int> set2;
        set2.emplace(25);
        set2.emplace(10);
        set2.emplace(34);
        set2.emplace(65);
        set2.emplace(35);
        set2.emplace(10);
        CHECK(set2.size() == 6);
        set2.erase(10);
        CHECK(set2.size() == 4);
    }

    TEST_CASE("FlatMapTest")
    {
        tlx::flat_map<int, int> map1;
        map1.emplace(25, 66);
        map1.emplace(10, 75);
        map1.emplace(34, 32);
        map1.emplace(65, 31);
        map1.emplace(35, 39);
        map1.emplace(10, 34);

        CHECK(map1.size() == 5);
        map1.erase(10);
        CHECK(map1.size() == 4);

        tlx::flat_multimap<int, int> map2;
        map2.emplace(25, 66);
        map2.emplace(10, 75);
        map2.emplace(34, 32);
        map2.emplace(65, 31);
        map2.emplace(35, 39);
        map2.emplace(10, 34);
        CHECK(map2.size() == 6);
        map2.erase(10);
        CHECK(map2.size() == 4);
    }

    TEST_CASE("BenchmarkTest")
    {
        run_benchmark();
    }

    using namespace ax;

    TEST_CASE("IndexArray default constructor uses U_SHORT")
    {
        IndexArray arr;
        CHECK(arr.format() == rhi::IndexFormat::U_SHORT);
        CHECK(arr.empty());
        CHECK(arr.size() == 0);
    }

    TEST_CASE("IndexArray initializer list with uint16_t")
    {
        IndexArray arr{ilist_u16_t{1, 2, 3}};
        CHECK(arr.format() == rhi::IndexFormat::U_SHORT);
        CHECK(arr.size() == 3);
        CHECK(arr.at<uint16_t>(0) == 1);
        CHECK(arr.at<uint16_t>(1) == 2);
        CHECK(arr.at<uint16_t>(2) == 3);
    }

    TEST_CASE("IndexArray initializer list with uint32_t")
    {
        IndexArray arr{ilist_u32_t{100, 200, 300}};
        CHECK(arr.format() == rhi::IndexFormat::U_INT);
        CHECK(arr.size() == 3);
        CHECK(arr.at<uint32_t>(0) == 100);
        CHECK(arr.at<uint32_t>(1) == 200);
        CHECK(arr.at<uint32_t>(2) == 300);
    }

    TEST_CASE("IndexArray emplace_back and operator[]")
    {
        IndexArray arr(rhi::IndexFormat::U_SHORT);
        arr.emplace_back<uint16_t>(42);
        arr.emplace_back<uint16_t>(99);
        CHECK(arr.size() == 2);
        CHECK(arr[uint16_t(0)] == 42);
        CHECK(arr[uint16_t(1)] == 99);

        arr[uint16_t(1)] = 123;
        CHECK(arr.at<uint16_t>(1) == 123);
    }

    TEST_CASE("IndexArray erase single element")
    {
        IndexArray arr{ilist_u16_t{10, 20, 30}};
        auto* pos = arr.begin<uint16_t>() + 1;
        arr.erase<uint16_t>(pos);
        CHECK(arr.size() == 2);
        CHECK(arr.at<uint16_t>(0) == 10);
        CHECK(arr.at<uint16_t>(1) == 30);
    }

    TEST_CASE("IndexArray erase range")
    {
        IndexArray arr{ilist_u16_t{10, 20, 30, 40, 50, 60, 70, 80, 90, 100}};
        auto* pos = arr.begin<uint16_t>() + 1;
        arr.erase<uint16_t>(pos, pos + 5);
        CHECK(arr.size() == 5);
        CHECK(arr.at<uint16_t>(0) == 10);
        CHECK(arr.at<uint16_t>(1) == 70);
    }

    TEST_CASE("IndexArray resize and clear")
    {
        IndexArray arr(rhi::IndexFormat::U_INT);
        arr.resize(5);
        CHECK(arr.size() == 5);
        arr.clear();
        CHECK(arr.empty());
    }

    TEST_CASE("IndexArray for_each callback")
    {
        IndexArray arr{ilist_u16_t{1, 2, 3}};
        std::vector<uint32_t> values;
        arr.for_each([&](uint32_t v) { values.push_back(v); });
        CHECK(values == std::vector<uint32_t>{1, 2, 3});
    }
}
