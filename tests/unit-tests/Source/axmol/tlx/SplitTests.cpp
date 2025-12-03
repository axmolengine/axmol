#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <ranges>
#include "axmol/tlx/split.hpp"

// -----------------------------
// Helpers
// -----------------------------

// Collect results using tlx::split (char delimiter)
// Note: call the overload taking Str&& and char, not template hacks.
// using my_char_type = tlx::element_of_nocvref_t<const char*>;
auto collect_tlx_split_unsafe(const char* s, char delim)
{
    std::vector<std::string> results;
    tlx::split(s, delim, [&](const char* first, const char* last) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);  // nullptr means until end
    });
    return results;
}
// Collect results using tlx::split (string_view delimiter)
auto collect_tlx_split_safe(std::string_view s, char delim)
{
    std::vector<std::string> results;
    tlx::split(s, delim, [&](const char* first, const char* last) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);
    });
    return results;
}

auto collect_tlx_split_unsafe(const char* s, std::string_view delim)
{
    std::vector<std::string> results;
    tlx::split(s, delim, [&](const char* first, const char* last) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);  // nullptr means until end
    });
    return results;
}

auto collect_tlx_split_safe(std::string_view s, std::string_view delim)
{
    std::vector<std::string> results;
    tlx::split(s, delim, [&](const char* first, const char* last) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);
    });
    return results;
}

// Collect results using tlx::split_of (multiple char delimiters)
auto collect_tlx_split_of_unsafe(const char* s, std::string_view delims)
{
    std::vector<std::string> results;
    tlx::split_of(s, delims, [&](const char* first, const char* last, char /*delim*/) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);
    });
    return results;
}

auto collect_tlx_split_of_safe(std::string_view s, std::string_view delims)
{
    std::vector<std::string> results;
    tlx::split_of(s, delims, [&](const char* first, const char* last, char /*delim*/) {
        if (last)
            results.emplace_back(first, last);
        else
            results.emplace_back(first);
    });
    return results;
}

// Collect results using std::views::split (single delimiter)
auto collect_std_split(const std::string& s, char delim)
{
    std::vector<std::string> results;
    for (auto&& sub : s | std::views::split(delim))
    {
        results.emplace_back(sub.begin(), sub.end());
    }
    return results;
}

// Collect results using std::views::split (string_view delimiter)
auto collect_std_split_sv(const std::string& s, std::string_view delim)
{
    std::vector<std::string> results;
    for (auto&& sub : s | std::views::split(delim))
    {
        results.emplace_back(sub.begin(), sub.end());
    }
    return results;
}

// Simulated std::split_of (multiple delimiters)
auto collect_std_split_of(const std::string& s, std::string_view delims)
{
    std::vector<std::string> results;
    std::string current;
    for (char ch : s)
    {
        if (delims.find(ch) != std::string_view::npos)
        {
            results.push_back(current);
            current.clear();
        }
        else
        {
            current.push_back(ch);
        }
    }
    results.push_back(current);
    return results;
}

TEST_SUITE("tlx/SplitTests")
{

    // -----------------------------
    // tlx::split vs std::views::split (char delimiter)
    // -----------------------------

    TEST_CASE("split basic (char delimiter)")
    {
        std::string s = "a,b,c";
        auto tlx_res  = collect_tlx_split_unsafe(s.c_str(), ',');
        auto std_res  = collect_std_split(s, ',');
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_safe(s, ',');
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split empty input (char delimiter)")
    {
        std::string s;
        auto tlx_res = collect_tlx_split_unsafe(s.c_str(), ',');
        auto std_res = collect_std_split(s, ',');
        CHECK(tlx_res.size() == 1);

        tlx_res = collect_tlx_split_safe(s, ',');
        AXLOGI("c++ std::views::split empty input (char delimiter) subrange count: {}", std_res.size());
        CHECK(tlx_res.size() == 1);
    }

    TEST_CASE("split leading delimiter (char delimiter)")
    {
        std::string s = ",a,b";
        auto tlx_res  = collect_tlx_split_unsafe(s.c_str(), ',');
        auto std_res  = collect_std_split(s, ',');
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_safe(s, ',');
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split trailing delimiter (char delimiter)")
    {
        std::string s = "a,b,";
        auto tlx_res  = collect_tlx_split_unsafe(s.c_str(), ',');
        auto std_res  = collect_std_split(s, ',');
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_safe(s.c_str(), ',');
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split consecutive delimiters (char delimiter)")
    {
        std::string s = "a,,b";
        auto tlx_res  = collect_tlx_split_unsafe(s.c_str(), ',');
        auto std_res  = collect_std_split(s, ',');
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_safe(s, ',');
        CHECK(tlx_res == std_res);
    }

    // -----------------------------
    // tlx::split vs std::views::split (string_view delimiter)
    // -----------------------------

    TEST_CASE("split basic (string_view delimiter)")
    {
        std::string s          = "a::b::c";
        std::string_view delim = "::";
        auto tlx_res           = collect_tlx_split_safe(s, delim);
        auto std_res           = collect_std_split_sv(s, delim);
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split leading delimiter (string_view delimiter)")
    {
        std::string s          = "::a::b";
        std::string_view delim = "::";
        auto tlx_res           = collect_tlx_split_safe(s.c_str(), delim);
        auto std_res           = collect_std_split_sv(s, delim);
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split trailing delimiter (string_view delimiter)")
    {
        std::string s          = "a::b::";
        std::string_view delim = "::";
        auto tlx_res           = collect_tlx_split_safe(s.c_str(), delim);
        auto std_res           = collect_std_split_sv(s, delim);
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split consecutive delimiters (string_view delimiter)")
    {
        std::string s          = "a::::b";
        std::string_view delim = "::";
        auto tlx_res           = collect_tlx_split_safe(s.c_str(), delim);
        auto std_res           = collect_std_split_sv(s, delim);
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split empty input (string_view delimiter)")
    {
        std::string s;
        std::string_view delim = "::";
        auto tlx_res           = collect_tlx_split_safe(s.c_str(), delim);
        auto std_res           = collect_std_split_sv(s, delim);

        AXLOGI("c++ std::views::split empty input (string_view delimiter) subrange count: {}", std_res.size());
        CHECK(tlx_res.size() == 1);
    }

    // -----------------------------
    // tlx::split iterator-range overload (char delimiter) vs std::views::split
    // -----------------------------

    TEST_CASE("split iterator range (char delimiter)")
    {
        std::string s = "x|y|z";
        std::vector<std::string> tlx_res;
        tlx::split(s.begin(), s.end(), '|', [&](const char* f, const char* l) { tlx_res.emplace_back(f, l); });
        auto std_res = collect_std_split(s, '|');
        CHECK(tlx_res == std_res);
    }

    // -----------------------------
    // tlx::split_of vs simulated std_split_of (multiple delimiters)
    // -----------------------------

    TEST_CASE("split_of basic multiple delimiters")
    {
        std::string s = "a,b;c";
        auto tlx_res  = collect_tlx_split_of_unsafe(s.c_str(), ",;");
        auto std_res  = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_of_safe(s, ",;");
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split_of leading delimiter")
    {
        std::string s = ",a;b";
        auto tlx_res  = collect_tlx_split_of_unsafe(s.c_str(), ",;");
        auto std_res  = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_of_safe(s, ",;");
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split_of trailing delimiter")
    {
        std::string s = "a;b,";
        auto tlx_res  = collect_tlx_split_of_unsafe(s.c_str(), ",;");
        auto std_res  = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_of_safe(s, ",;");
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split_of consecutive delimiters")
    {
        std::string s = "a,,;b";
        auto tlx_res  = collect_tlx_split_of_unsafe(s.c_str(), ",;");
        auto std_res  = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);

        tlx_res = collect_tlx_split_of_safe(s, ",;");
        CHECK(tlx_res == std_res);
    }

    TEST_CASE("split_of empty input")
    {
        std::string s;
        auto tlx_res = collect_tlx_split_of_unsafe(s.c_str(), ",;");
        auto std_res = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);
        tlx_res = collect_tlx_split_of_safe(s, ",;");

        CHECK(tlx_res.size() == std_res.size());
    }

    // -----------------------------
    // tlx::split_of iterator-range overload vs simulated std_split_of
    // -----------------------------

    TEST_CASE("split_of iterator range (multiple delimiters)")
    {
        std::string s = "A,B;C,,;D,";
        std::vector<std::string> tlx_res;
        tlx::split_of(s.begin(), s.end(), std::string_view(",;"),
                      [&](const char* f, const char* l, char /*delim*/) { tlx_res.emplace_back(f, l); });
        auto std_res = collect_std_split_of(s, ",;");
        CHECK(tlx_res == std_res);
    }
}
