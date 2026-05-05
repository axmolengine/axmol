#pragma once
#include "axmol/tlx/bitmask.hpp"

namespace ax
{
enum class ContactEventBits : uint32_t
{
    None     = 0,
    PreSolve = 1 << 0,
    Hit      = 1 << 1,
    Contact  = 1 << 2,
    Sensor   = 1 << 3,
    AllBits  = PreSolve | Hit | Contact | Sensor
};

AX_ENABLE_BITMASK_OPS(ContactEventBits);

}  // namespace ax
