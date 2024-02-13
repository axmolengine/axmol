#include "Margin.h"

NS_FGUI_BEGIN

const Margin Margin::ZERO = Margin(0, 0, 0, 0);

Margin::Margin(void) : left(0), top(0), right(0), bottom(0)
{
}

Margin::Margin(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b)
{
}

Margin::Margin(const Margin& other) : left(other.left), top(other.top), right(other.right), bottom(other.bottom)
{
}

Margin& Margin::operator= (const Margin& other)
{
    setMargin(other.left, other.top, other.right, other.bottom);
    return *this;
}

void Margin::setMargin(float l, float t, float r, float b)
{
    left = l;
    top = t;
    right = r;
    bottom = b;
}

bool Margin::equals(const Margin &target) const
{
    return (left == target.left && top == target.top && right == target.right && bottom == target.bottom);
}

NS_FGUI_END