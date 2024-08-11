#include <doctest.h>
#include "base/Types.h"
#include "TestUtils.h"

NS_AX_BEGIN


doctest::String toString(const Vec2& value) {
    std::string s;
    s.append("(");
    s.append(std::to_string(value.u));
    s.append(", ");
    s.append(std::to_string(value.v));
    s.append(")");
    return s.c_str();
}

doctest::String toString(const Vec3& value) {
    std::string s;
    s.append("(");
    s.append(std::to_string(value.x));
    s.append(", ");
    s.append(std::to_string(value.y));
    s.append(", ");
    s.append(std::to_string(value.z));
    s.append(")");
    return s.c_str();
}

doctest::String toString(const Color4B& value) {
    std::string s;
    s.append("(");
    s.append(std::to_string(value.r));
    s.append(", ");
    s.append(std::to_string(value.g));
    s.append(", ");
    s.append(std::to_string(value.b));
    s.append(", ");
    s.append(std::to_string(value.a));
    s.append(")");
    return s.c_str();
}


NS_AX_END
