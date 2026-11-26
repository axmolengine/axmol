/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include <doctest.h>
#include <array>
#include <stdint.h>
#include <vector>
#include "axmol/rhi/RHIUtils.h"

using namespace ax::rhi;

TEST_SUITE("rhi/RHIUtils")
{
    TEST_CASE("convertBGRA8ToRGBA8 out-of-place")
    {
        const std::vector<uint8_t> src = {
            0x10, 0x20, 0x30, 0x40,  // B=0x10, G=0x20, R=0x30, A=0x40
            0xFF, 0x00, 0x00, 0x80,  // pure blue in BGRA -> RGBA(0, 0, 255, 128)
            0x00, 0x00, 0xFF, 0xFF,  // pure red in BGRA -> RGBA(255, 0, 0, 255)
        };
        const std::vector<uint8_t> expected = {
            0x30, 0x20, 0x10, 0x40,  // R=0x30, G=0x20, B=0x10, A=0x40
            0x00, 0x00, 0xFF, 0x80,
            0xFF, 0x00, 0x00, 0xFF,
        };

        std::vector<uint8_t> dst(src.size(), 0);
        RHIUtils::convertBGRA8ToRGBA8(src.data(), src.size(), dst.data());
        CHECK_EQ(dst, expected);
    }

    TEST_CASE("convertBGRA8ToRGBA8 in-place")
    {
        std::vector<uint8_t> buf = {
            0x01, 0x02, 0x03, 0x04,
            0xAA, 0xBB, 0xCC, 0xDD,
        };
        const std::vector<uint8_t> expected = {
            0x03, 0x02, 0x01, 0x04,
            0xCC, 0xBB, 0xAA, 0xDD,
        };

        RHIUtils::convertBGRA8ToRGBA8(buf.data(), buf.size(), buf.data());
        CHECK_EQ(buf, expected);
    }

    TEST_CASE("convertBGRA8ToRGBA8 edge cases and unaligned buffers")
    {
        // Null and short lengths should be safe no-ops
        std::array<uint8_t, 5> smallBuf = {1, 2, 3, 4, 5};
        RHIUtils::convertBGRA8ToRGBA8(nullptr, 4, smallBuf.data());
        RHIUtils::convertBGRA8ToRGBA8(smallBuf.data(), 4, nullptr);
        RHIUtils::convertBGRA8ToRGBA8(smallBuf.data(), 0, smallBuf.data());
        RHIUtils::convertBGRA8ToRGBA8(smallBuf.data(), 3, smallBuf.data());
        CHECK_EQ(smallBuf, std::array<uint8_t, 5>{1, 2, 3, 4, 5});

        // Non-multiple-of-4 trailing byte must remain untouched
        RHIUtils::convertBGRA8ToRGBA8(smallBuf.data(), smallBuf.size(), smallBuf.data());
        CHECK_EQ(smallBuf, std::array<uint8_t, 5>{3, 2, 1, 4, 5});

        // Unaligned pointers (offset +1)
        std::array<uint8_t, 9> unalignedSrc = {0x99, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        std::array<uint8_t, 9> unalignedDst = {0xEE, 0, 0, 0, 0, 0, 0, 0, 0};
        RHIUtils::convertBGRA8ToRGBA8(unalignedSrc.data() + 1, 8, unalignedDst.data() + 1);
        CHECK_EQ(unalignedDst, std::array<uint8_t, 9>{0xEE, 0x33, 0x22, 0x11, 0x44, 0x77, 0x66, 0x55, 0x88});
    }
}
