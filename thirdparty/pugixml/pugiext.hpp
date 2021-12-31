#pragma once

#include <functional>
#include <string>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace pugiext
{
template <typename _NumberType>
inline bool query_attribute(pugi::xml_node& node, const char* aname, _NumberType* value)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = ds.as_int();
        return true;
    }
    return false;
}
template <>
inline bool query_attribute<float>(pugi::xml_node& node, const char* aname, float* value)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = ds.as_float();
        return true;
    }
    return false;
}
template <>
inline bool query_attribute<bool>(pugi::xml_node& node, const char* aname, bool* value)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = ds.as_bool();
        return true;
    }
    return false;
}
template <>
inline bool query_attribute<std::string>(pugi::xml_node& node, const char* aname, std::string* value)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = ds.as_string();
        return true;
    }
    return false;
}
template <>
inline bool query_attribute<const char*>(pugi::xml_node& node, const char* aname, const char** value)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = ds.as_string().data();
        return true;
    }
    return false;
}

inline bool query_attribute(pugi::xml_node& node, const char* aname, float* value, std::function<float(float)> vconv)
{
    auto ds = node.attribute(aname);
    if (ds)
    {
        *value = vconv(ds.as_float());
        return true;
    }
    return false;
}
}  // namespace pugiext
