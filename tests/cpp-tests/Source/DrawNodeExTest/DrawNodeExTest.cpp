/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd..
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "DrawNodeExTest.h"

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

USING_NS_AX;
USING_NS_AX_EXT;

using namespace std;

const int drawMethodsCounter = 19;
std::string drawMethods[drawMethodsCounter] = { "drawLine",
                                                "drawRect",
                                                "drawCircle",
                                                "drawQuadBezier",
                                                "drawCubicBezier",
                                                "drawCardinalSpline",
                                                "drawCatmullRom",
                                                "drawPoly",
                                                "drawPolygon",
                                                "drawDot",
                                                "drawPoint",
                                                "drawPoints",
                                                "drawTriangle",
                                                "drawSegment",
                                                "drawSolidCircle",
                                                "drawSolidPoly",
                                                "drawSolidRect",
                                                "drawStar",
                                                "drawSolidStar", };



Vec2 vertices1[] = {
    {45.750000f, 144.375000f},  {75.500000f, 136.875000f},  {75.500000f, 159.125000f},  {100.250000f, 161.375000f},
    {65.500000f, 181.375000f},  {102.250000f, 179.125000f}, {95.000000f, 215.125000f},  {129.331467f, 189.926208f},
    {131.371460f, 206.366196f}, {139.651474f, 192.446198f}, {161.851471f, 200.606201f}, {151.000000f, 220.375000f},
    {110.500000f, 244.375000f}, {153.750000f, 238.125000f}, {142.500000f, 253.875000f}, {220.750000f, 259.375000f},
    {250.500000f, 244.375000f}, {168.750000f, 241.875000f}, {182.250000f, 154.125000f}, {190.250000f, 227.375000f},
    {196.500000f, 197.375000f}, {208.750000f, 210.625000f}, {220.750000f, 194.375000f}, {208.750000f, 176.375000f},
    {253.250000f, 173.875000f}, {243.750000f, 154.125000f}, {213.750000f, 161.375000f}, {202.250000f, 139.875000f},
    {236.000000f, 131.875000f}, {218.500000f, 120.875000f}, {206.500000f, 125.625000f}, {184.500000f, 110.375000f},
    {157.000000f, 108.625000f}, {147.500000f, 96.625000f},  {153.750000f, 85.125000f},  {147.500000f, 75.375000f},
    {126.500000f, 74.125000f},  {110.500000f, 86.625000f},  {127.750000f, 85.125000f},  {135.250000f, 91.125000f},
    {135.250000f, 97.875000f},  {124.000000f, 93.875000f},  {115.500000f, 100.875000f}, {115.500000f, 111.875000f},
    {135.250000f, 108.625000f}, {151.000000f, 124.125000f}, {90.500000f, 131.875000f},  {113.250000f, 120.875000f},
    {88.000000f, 116.875000f},  {106.000000f, 103.875000f}, {88.000000f, 97.875000f} };

Vec2 vertices2[] = {
    {290.250000f, 98.1250000f}, {235.000000f, 90.8750000f}, {270.500000f, 109.875000f}, {235.000000f, 119.125000f},
    {275.250000f, 145.875000f}, {249.500000f, 145.875000f}, {249.500000f, 178.125000f}, {275.250000f, 187.375015f},
    {294.750488f, 168.333344f}, {311.250000f, 181.125000f}, {257.000000f, 213.625015f}, {338.500000f, 193.125000f},
    {300.000000f, 211.125015f}, {333.750000f, 211.125015f}, {368.250000f, 206.625000f}, {377.000000f, 178.125000f},
    {421.750000f, 170.125000f}, {416.250000f, 115.375000f}, {391.250000f, 157.875000f}, {338.500000f, 131.625000f},
    {362.750000f, 131.625000f}, {362.750000f, 106.875000f}, {306.500000f, 119.125000f}, {324.250000f, 85.1250000f},
    {227.500000f, 61.8750000} };


// Original https :  // www.purebasic.fr/english/viewtopic.php?t=82915
float verticesHead[] = { 107.f, 9.f, 0.3333333433f, 0.3411764801f, 0.3686274588f, 255.f, 81.f, 599.f, 116.f, 571.f, 180.f, 562.f, 255.f, 559.f, 213.f, 586.f, 199.f, 599.f, 0.f,  7.f, 0.3333333433f, 0.3411764801f, 0.3686274588f, 255.f, 765.f, 584.f, 782.f, 590.f, 794.f, 599.f, 772.f, 599.f, 0.f,  13.f, 0.4156862795f, 0.4313725531f, 0.4549019635f, 255.f, 278.f, 573.f, 287.f, 599.f, 199.f, 599.f, 217.f, 582.f, 256.f, 557.f, 313.f, 532.f, 352.f, 508.f, 366.f, 512.f, 368.f, 545.f, 285.f, 598.f, 0.f,  8.f, 0.4156862795f, 0.4313725531f, 0.4549019635f, 255.f, 760.f, 591.f, 754.f, 579.f, 766.f, 584.f, 773.f, 599.f, 751.f, 599.f, 0.f,  12.f, 0.400000006f, 0.4117647111f, 0.4196078479f, 255.f, 701.f, 563.f, 615.f, 599.f, 652.f, 558.f, 705.f, 507.f, 716.f, 534.f, 753.f, 577.f, 760.f, 591.f, 752.f, 599.f, 613.f, 599.f, 0.f,  10.f, 0.400000006f, 0.4117647111f, 0.4196078479f, 255.f, 704.f, 506.f, 701.f, 493.f, 686.f, 488.f, 688.f, 503.f, 692.f, 514.f, 691.f, 528.f, 708.f, 518.f, 0.f,  8.f, 0.4784313738f, 0.3647058904f, 0.2156862766f, 255.f, 285.f, 599.f, 309.f, 579.f, 365.f, 546.f, 375.f, 578.f, 378.f, 599.f, 0.f,  16.f, 0.4784313738f, 0.3647058904f, 0.2156862766f, 255.f, 471.f, 599.f, 377.f, 599.f, 365.f, 546.f, 360.f, 511.f, 394.f, 515.f, 428.f, 507.f, 492.f, 471.f, 553.f, 417.f, 568.f, 397.f, 562.f, 422.f, 552.f, 462.f, 547.f, 487.f, 524.f, 529.f, 0.f,  19.f, 0.5921568871f, 0.4980392158f, 0.3647058904f, 255.f, 614.f, 599.f, 683.f, 531.f, 690.f, 512.f, 686.f, 493.f, 684.f, 429.f, 656.f, 245.f, 654.f, 242.f, 644.f, 273.f, 628.f, 288.f, 621.f, 288.f, 592.f, 308.f, 571.f, 378.f, 554.f, 454.f, 543.f, 494.f, 514.f, 542.f, 469.f, 599.f, 0.f,  12.f, 0.5921568871f, 0.4980392158f, 0.3647058904f, 255.f, 598.f, 259.f, 602.f, 233.f, 619.f, 236.f, 624.f, 250.f, 630.f, 257.f, 629.f, 269.f, 623.f, 291.f, 603.f, 312.f, 589.f, 311.f, 0.f,  15.f, 0.4784313738f, 0.3647058904f, 0.2156862766f, 255.f, 642.f, 211.f, 606.f, 197.f, 609.f, 236.f, 621.f, 236.f, 621.f, 246.f, 630.f, 256.f, 627.f, 279.f, 626.f, 286.f, 636.f, 282.f, 641.f, 274.f, 649.f, 250.f, 656.f, 221.f, 0.f,  24.f, 0.4784313738f, 0.3647058904f, 0.2156862766f, 255.f, 309.f, 333.f, 334.f, 269.f, 346.f, 207.f, 347.f, 169.f, 323.f, 42.f, 265.f, 39.f, 256.f, 156.f, 272.f, 212.f, 276.f, 302.f, 292.f, 372.f, 323.f, 469.f, 334.f, 500.f, 350.f, 509.f, 365.f, 511.f, 358.f, 472.f, 370.f, 451.f, 382.f, 438.f, 382.f, 433.f, 389.f, 392.f, 382.f, 309.f, 315.f, 291.f, 0.f,  35.f, 0.5921568871f, 0.4980392158f, 0.3647058904f, 255.f, 439.f, 399.f, 384.f, 393.f, 354.f, 396.f, 349.f, 395.f, 348.f, 405.f, 349.f, 421.f, 352.f, 428.f, 363.f, 434.f, 383.f, 435.f, 364.f, 458.f, 358.f, 473.f, 360.f, 499.f, 364.f, 512.f, 382.f, 516.f, 407.f, 514.f, 437.f, 506.f, 476.f, 482.f, 534.f, 437.f, 562.f, 408.f, 571.f, 382.f, 581.f, 346.f, 598.f, 0.f, 333.f, 0.f, 345.f, 190.f, 336.f, 249.f, 379.f, 333.f, 373.f, 347.f, 347.f, 353.f, 347.f, 371.f, 354.f, 377.f, 387.f, 384.f, 389.f, 396.f, 0.f,  7.f, 0.5921568871f, 0.4980392158f, 0.3647058904f, 255.f, 370.f, 309.f, 322.f, 296.f, 333.f, 268.f, 339.f, 238.f, 0.f,  10.f, 0.5921568871f, 0.4980392158f, 0.3647058904f, 255.f, 341.f, 289.f, 323.f, 293.f, 314.f, 317.f, 324.f, 328.f, 352.f, 326.f, 390.f, 332.f, 388.f, 288.f, 0.f,  12.f, 0.4784313738f, 0.3647058904f, 0.2156862766f, 255.f, 440.f, 145.f, 520.f, 146.f, 464.f, 167.f, 429.f, 220.f, 422.f, 222.f, 400.f, 206.f, 392.f, 188.f, 390.f, 173.f, 393.f, 160.f, 0.f,  15.f, 0.7960784435f, 0.7490196228f, 0.6705882549f, 255.f, 460.f, 194.f, 489.f, 189.f, 500.f, 199.f, 499.f, 211.f, 489.f, 217.f, 467.f, 223.f, 445.f, 224.f, 431.f, 217.f, 422.f, 209.f, 420.f, 200.f, 436.f, 181.f, 491.f, 189.f, 0.f,  12.f, 0.7960784435f, 0.7490196228f, 0.6705882549f, 255.f, 342.f, 199.f, 343.f, 211.f, 337.f, 218.f, 322.f, 222.f, 306.f, 221.f, 292.f, 212.f, 288.f, 197.f, 297.f, 186.f, 332.f, 189.f, 0.f,  13.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 461.f, 182.f, 445.f, 184.f, 442.f, 196.f, 446.f, 210.f, 454.f, 218.f, 462.f, 219.f, 472.f, 217.f, 480.f, 207.f, 480.f, 196.f, 477.f, 185.f, 0.f,  11.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 309.f, 186.f, 306.f, 192.f, 307.f, 204.f, 313.f, 213.f, 325.f, 216.f, 334.f, 210.f, 336.f, 197.f, 332.f, 189.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 433.f, 186.f, 422.f, 199.f, 416.f, 199.f, 432.f, 179.f, 475.f, 183.f, 470.f, 185.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 470.f, 183.f, 491.f, 187.f, 497.f, 194.f, 482.f, 188.f, 433.f, 184.f, 436.f, 181.f, 0.f,  10.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 331.f, 190.f, 340.f, 199.f, 342.f, 197.f, 333.f, 188.f, 296.f, 184.f, 292.f, 186.f, 296.f, 186.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 299.f, 187.f, 291.f, 198.f, 287.f, 198.f, 296.f, 185.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 312.f, 176.f, 297.f, 178.f, 297.f, 181.f, 312.f, 177.f, 326.f, 181.f, 326.f, 179.f, 0.f,  11.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 439.f, 172.f, 470.f, 174.f, 488.f, 180.f, 457.f, 176.f, 432.f, 176.f, 423.f, 180.f, 423.f, 178.f, 431.f, 173.f, 0.f,  10.f, 0.7843137383f, 0.7254902124f, 0.6078431606f, 255.f, 467.f, 186.f, 464.f, 189.f, 464.f, 194.f, 467.f, 199.f, 473.f, 198.f, 477.f, 193.f, 477.f, 188.f, 0.f,  10.f, 0.7843137383f, 0.7254902124f, 0.6078431606f, 255.f, 332.f, 191.f, 334.f, 194.f, 332.f, 198.f, 327.f, 198.f, 323.f, 196.f, 322.f, 192.f, 324.f, 188.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 398.f, 195.f, 399.f, 204.f, 401.f, 208.f, 400.f, 198.f, 400.f, 194.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 365.f, 434.f, 350.f, 427.f, 350.f, 427.f, 356.f, 433.f, 372.f, 437.f, 382.f, 434.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 389.f, 393.f, 418.f, 401.f, 420.f, 403.f, 390.f, 396.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 391.f, 397.f, 355.f, 397.f, 356.f, 396.f, 388.f, 393.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 354.f, 397.f, 344.f, 393.f, 343.f, 390.f, 355.f, 396.f, 358.f, 396.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 343.f, 392.f, 335.f, 393.f, 337.f, 391.f, 344.f, 389.f, 346.f, 392.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 354.f, 379.f, 345.f, 372.f, 346.f, 370.f, 355.f, 377.f, 361.f, 373.f, 361.f, 377.f, 0.f,  11.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 368.f, 336.f, 375.f, 337.f, 373.f, 340.f, 367.f, 337.f, 360.f, 340.f, 356.f, 346.f, 356.f, 342.f, 363.f, 336.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 338.f, 337.f, 348.f, 351.f, 345.f, 351.f, 336.f, 339.f, 324.f, 327.f, 329.f, 328.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 367.f, 325.f, 326.f, 327.f, 330.f, 330.f, 367.f, 327.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 367.f, 326.f, 378.f, 331.f, 378.f, 328.f, 367.f, 325.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 325.f, 327.f, 316.f, 317.f, 315.f, 319.f, 322.f, 328.f, 329.f, 332.f, 329.f, 329.f, 0.f,  10.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 336.f, 256.f, 329.f, 284.f, 315.f, 312.f, 316.f, 312.f, 331.f, 267.f, 343.f, 213.f, 345.f, 214.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 654.f, 243.f, 644.f, 272.f, 635.f, 283.f, 647.f, 261.f, 651.f, 238.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 634.f, 282.f, 617.f, 290.f, 608.f, 301.f, 625.f, 287.f, 634.f, 286.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 605.f, 310.f, 609.f, 302.f, 616.f, 294.f, 610.f, 295.f, 605.f, 307.f, 0.f,  10.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 556.f, 578.f, 569.f, 550.f, 569.f, 544.f, 556.f, 576.f, 539.f, 599.f, 543.f, 599.f, 559.f, 576.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 572.f, 548.f, 582.f, 507.f, 582.f, 496.f, 583.f, 496.f, 577.f, 537.f, 575.f, 552.f, 0.f,  10.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 588.f, 497.f, 589.f, 474.f, 590.f, 468.f, 592.f, 467.f, 589.f, 499.f, 582.f, 527.f, 581.f, 521.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 572.f, 376.f, 574.f, 376.f, 563.f, 411.f, 560.f, 411.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 562.f, 406.f, 540.f, 432.f, 552.f, 422.f, 564.f, 409.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 547.f, 425.f, 504.f, 463.f, 501.f, 463.f, 530.f, 443.f, 551.f, 424.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 503.f, 462.f, 467.f, 488.f, 468.f, 489.f, 502.f, 465.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 464.f, 489.f, 438.f, 504.f, 438.f, 506.f, 452.f, 501.f, 470.f, 490.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 441.f, 503.f, 406.f, 513.f, 406.f, 515.f, 431.f, 509.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 409.f, 513.f, 377.f, 513.f, 377.f, 516.f, 397.f, 516.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 378.f, 514.f, 357.f, 510.f, 358.f, 513.f, 383.f, 517.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 622.f, 282.f, 623.f, 289.f, 625.f, 288.f, 625.f, 281.f, 632.f, 269.f, 631.f, 264.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 631.f, 267.f, 630.f, 257.f, 627.f, 247.f, 629.f, 266.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 628.f, 251.f, 621.f, 239.f, 618.f, 239.f, 624.f, 250.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 620.f, 241.f, 621.f, 231.f, 619.f, 229.f, 617.f, 241.f, 0.f,  10.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 620.f, 231.f, 627.f, 226.f, 633.f, 218.f, 633.f, 215.f, 624.f, 226.f, 619.f, 229.f, 617.f, 237.f, 0.f,  8.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 636.f, 274.f, 638.f, 260.f, 635.f, 252.f, 638.f, 267.f, 635.f, 275.f, 0.f,  11.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 631.f, 251.f, 636.f, 257.f, 636.f, 259.f, 639.f, 257.f, 634.f, 250.f, 625.f, 249.f, 628.f, 253.f, 634.f, 253.f, 0.f,  11.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 598.f, 339.f, 595.f, 334.f, 594.f, 328.f, 597.f, 328.f, 601.f, 337.f, 611.f, 339.f, 611.f, 341.f, 602.f, 342.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 341.f, 198.f, 339.f, 187.f, 337.f, 187.f, 338.f, 197.f, 0.f,  9.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 360.f, 512.f, 364.f, 546.f, 378.f, 599.f, 379.f, 599.f, 374.f, 572.f, 366.f, 547.f, 0.f,  7.f, 0.4470588267f, 0.3764705956f, 0.2666666806f, 255.f, 373.f, 583.f, 370.f, 599.f, 378.f, 599.f, 374.f, 577.f, 0.f,  19.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 684.f, 453.f, 685.f, 478.f, 691.f, 473.f, 701.f, 460.f, 717.f, 414.f, 713.f, 382.f, 709.f, 374.f, 724.f, 312.f, 718.f, 299.f, 758.f, 118.f, 754.f, 83.f, 685.f, 48.f, 657.f, 247.f, 654.f, 281.f, 652.f, 289.f, 621.f, 328.f, 0.f,  10.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 710.f, 443.f, 708.f, 402.f, 718.f, 405.f, 713.f, 443.f, 708.f, 460.f, 706.f, 460.f, 710.f, 438.f, 0.f,  11.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 726.f, 378.f, 732.f, 402.f, 734.f, 419.f, 732.f, 420.f, 727.f, 392.f, 720.f, 381.f, 702.f, 371.f, 698.f, 344.f, 0.f,  14.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 722.f, 308.f, 730.f, 328.f, 728.f, 354.f, 725.f, 365.f, 726.f, 348.f, 722.f, 359.f, 721.f, 361.f, 717.f, 339.f, 716.f, 357.f, 712.f, 359.f, 706.f, 282.f, 0.f,  10.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 730.f, 292.f, 742.f, 281.f, 744.f, 275.f, 740.f, 278.f, 727.f, 287.f, 706.f, 292.f, 712.f, 301.f, 0.f,  11.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 719.f, 284.f, 729.f, 274.f, 748.f, 237.f, 758.f, 208.f, 761.f, 182.f, 761.f, 142.f, 757.f, 110.f, 691.f, 262.f, 0.f,  10.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 661.f, 433.f, 666.f, 445.f, 672.f, 456.f, 671.f, 453.f, 668.f, 433.f, 674.f, 424.f, 643.f, 349.f, 0.f,  10.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 646.f, 432.f, 662.f, 407.f, 655.f, 356.f, 649.f, 412.f, 642.f, 431.f, 634.f, 443.f, 636.f, 444.f, 0.f,  14.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 643.f, 413.f, 647.f, 404.f, 658.f, 389.f, 644.f, 314.f, 637.f, 328.f, 641.f, 402.f, 636.f, 422.f, 633.f, 427.f, 634.f, 428.f, 642.f, 417.f, 650.f, 401.f, 0.f,  8.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 642.f, 385.f, 634.f, 377.f, 619.f, 329.f, 627.f, 322.f, 652.f, 323.f, 0.f,  16.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 622.f, 373.f, 632.f, 383.f, 636.f, 385.f, 636.f, 382.f, 627.f, 374.f, 626.f, 367.f, 633.f, 364.f, 622.f, 326.f, 609.f, 340.f, 610.f, 348.f, 612.f, 359.f, 623.f, 376.f, 631.f, 382.f, 0.f,  21.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 682.f, 97.f, 660.f, 248.f, 652.f, 242.f, 648.f, 234.f, 647.f, 227.f, 641.f, 223.f, 633.f, 216.f, 624.f, 206.f, 580.f, 194.f, 579.f, 179.f, 565.f, 159.f, 558.f, 133.f, 560.f, 123.f, 539.f, 93.f, 529.f, 68.f, 521.f, 16.f, 529.f, 0.f, 698.f, 0.f, 0.f,  13.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 514.f, 48.f, 517.f, 92.f, 519.f, 106.f, 521.f, 106.f, 519.f, 81.f, 521.f, 59.f, 527.f, 39.f, 526.f, 7.f, 520.f, 16.f, 516.f, 32.f, 0.f,  8.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 449.f, 59.f, 464.f, 36.f, 471.f, 16.f, 472.f, 1.f, 466.f, 13.f, 0.f,  8.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 472.f, 0.f, 468.f, 21.f, 460.f, 31.f, 461.f, 6.f, 458.f, 0.f, 0.f,  20.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 458.f, 0.f, 451.f, 26.f, 441.f, 52.f, 428.f, 76.f, 414.f, 92.f, 399.f, 103.f, 387.f, 116.f, 389.f, 113.f, 411.f, 75.f, 418.f, 45.f, 399.f, 76.f, 319.f, 131.f, 315.f, 112.f, 309.f, 98.f, 316.f, 83.f, 323.f, 51.f, 246.f, 0.f, 0.f,  12.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 327.f, 149.f, 339.f, 172.f, 349.f, 189.f, 354.f, 194.f, 352.f, 188.f, 347.f, 167.f, 350.f, 123.f, 353.f, 114.f, 318.f, 128.f, 0.f,  8.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 396.f, 77.f, 377.f, 96.f, 348.f, 121.f, 321.f, 131.f, 318.f, 116.f, 0.f,  15.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 299.f, 59.f, 305.f, 88.f, 312.f, 105.f, 312.f, 84.f, 315.f, 62.f, 327.f, 44.f, 326.f, 23.f, 121.f, 0.f, 287.f, 56.f, 290.f, 78.f, 299.f, 101.f, 302.f, 107.f, 0.f,  14.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 207.f, 0.f, 286.f, 50.f, 270.f, 86.f, 257.f, 144.f, 256.f, 244.f, 237.f, 220.f, 227.f, 206.f, 211.f, 191.f, 197.f, 161.f, 128.f, 56.f, 142.f, 0.f, 0.f,  12.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 179.f, 168.f, 196.f, 198.f, 210.f, 221.f, 212.f, 219.f, 201.f, 200.f, 196.f, 174.f, 202.f, 148.f, 169.f, 96.f, 171.f, 141.f, 0.f,  16.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 152.f, 157.f, 158.f, 178.f, 168.f, 198.f, 172.f, 206.f, 172.f, 202.f, 165.f, 178.f, 164.f, 154.f, 173.f, 121.f, 129.f, 48.f, 126.f, 78.f, 130.f, 108.f, 133.f, 121.f, 144.f, 146.f, 0.f,  10.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 254.f, 155.f, 269.f, 202.f, 272.f, 221.f, 273.f, 258.f, 262.f, 243.f, 259.f, 248.f, 240.f, 222.f, 0.f,  14.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 112.f, 33.f, 121.f, 12.f, 124.f, 0.f, 145.f, 0.f, 134.f, 34.f, 127.f, 38.f, 118.f, 58.f, 112.f, 92.f, 108.f, 100.f, 107.f, 86.f, 106.f, 63.f, 0.f,  24.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 298.f, 146.f, 293.f, 134.f, 284.f, 132.f, 272.f, 136.f, 272.f, 138.f, 267.f, 140.f, 264.f, 149.f, 266.f, 158.f, 272.f, 157.f, 274.f, 159.f, 289.f, 154.f, 303.f, 157.f, 324.f, 171.f, 334.f, 176.f, 340.f, 181.f, 332.f, 171.f, 330.f, 162.f, 328.f, 153.f, 311.f, 146.f, 298.f, 136.f, 290.f, 133.f, 0.f,  22.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 485.f, 139.f, 487.f, 130.f, 509.f, 129.f, 532.f, 137.f, 544.f, 152.f, 549.f, 168.f, 543.f, 164.f, 543.f, 168.f, 522.f, 159.f, 496.f, 155.f, 470.f, 156.f, 435.f, 164.f, 407.f, 173.f, 391.f, 171.f, 384.f, 168.f, 379.f, 156.f, 413.f, 151.f, 414.f, 148.f, 489.f, 129.f, 0.f,  16.f, 0.1882352978f, 0.1686274558f, 0.0862745121f, 255.f, 607.f, 191.f, 606.f, 241.f, 606.f, 275.f, 596.f, 314.f, 592.f, 314.f, 581.f, 350.f, 574.f, 364.f, 574.f, 364.f, 580.f, 328.f, 577.f, 326.f, 577.f, 312.f, 583.f, 247.f, 579.f, 164.f, 0.f,  13.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 756.f, 68.f, 754.f, 97.f, 749.f, 104.f, 734.f, 107.f, 715.f, 100.f, 701.f, 102.f, 696.f, 97.f, 682.f, 26.f, 661.f, 0.f, 736.f, 0.f, 0.f,  11.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 689.f, 76.f, 687.f, 88.f, 683.f, 99.f, 676.f, 99.f, 674.f, 78.f, 669.f, 39.f, 659.f, 0.f, 705.f, 0.f, 0.f,  9.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 652.f, 0.f, 651.f, 16.f, 646.f, 29.f, 642.f, 31.f, 638.f, 24.f, 631.f, 0.f, 0.f,  9.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 628.f, 0.f, 628.f, 20.f, 625.f, 27.f, 619.f, 29.f, 616.f, 19.f, 616.f, 0.f, 0.f,  12.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 415.f, 0.f, 403.f, 31.f, 392.f, 54.f, 376.f, 68.f, 364.f, 74.f, 362.f, 70.f, 365.f, 61.f, 390.f, 25.f, 400.f, 0.f, 0.f,  12.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 352.f, 42.f, 341.f, 66.f, 339.f, 82.f, 344.f, 81.f, 350.f, 70.f, 364.f, 44.f, 381.f, 21.f, 380.f, 16.f, 368.f, 19.f, 0.f,  11.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 344.f, 0.f, 365.f, 0.f, 353.f, 21.f, 341.f, 49.f, 332.f, 62.f, 329.f, 58.f, 329.f, 51.f, 344.f, 0.f, 0.f,  10.f, 0.1568627506f, 0.1686274558f, 0.2352941185f, 255.f, 333.f, 2.f, 329.f, 17.f, 323.f, 23.f, 318.f, 18.f, 319.f, 6.f, 320.f, 0.f, 331.f, 0.f, 0.f,  11.f, 0.1764705926f, 0.1960784346f, 0.2666666806f, 255.f, 760.f, 0.f, 761.f, 39.f, 757.f, 80.f, 750.f, 86.f, 709.f, 69.f, 692.f, 47.f, 692.f, 26.f, 696.f, 0.f, 0.f,  7.f, 0.160784319f, 0.1764705926f, 0.1882352978f, 255.f, 278.f, 599.f, 324.f, 554.f, 322.f, 557.f, 283.f, 599.f, 0.f,  9.f, 0.160784319f, 0.1764705926f, 0.1882352978f, 255.f, 758.f, 589.f, 753.f, 577.f, 755.f, 577.f, 762.f, 591.f, 756.f, 599.f, 754.f, 599.f, 0.f,  7.f, 0.160784319f, 0.1764705926f, 0.1882352978f, 255.f, 773.f, 599.f, 765.f, 583.f, 763.f, 583.f, 770.f, 599.f, 0.f,  10.f, 0.160784319f, 0.1764705926f, 0.1882352978f, 255.f, 694.f, 507.f, 694.f, 498.f, 691.f, 498.f, 694.f, 509.f, 690.f, 519.f, 693.f, 521.f, 696.f, 507.f, 0.f,  9.f, 0.160784319f, 0.1764705926f, 0.1882352978f, 255.f, 215.f, 583.f, 256.f, 556.f, 253.f, 556.f, 216.f, 579.f, 198.f, 599.f, 200.f, 599.f, 0.f,  0.f };

DrawNodeExTests::DrawNodeExTests()
{
   // ADD_TEST_CASE(DrawNodeHeartTest);
    ADD_TEST_CASE(DrawNodeMorphTest);
   // ADD_TEST_CASE(DrawNodeFireworkTest);
    ADD_TEST_CASE(DrawNodePictureTest);
    ADD_TEST_CASE(DrawNodeMethodesTest);
    ADD_TEST_CASE(DrawNodePerformaneTest);
    ADD_TEST_CASE(DrawNodeThicknessTest);
    ADD_TEST_CASE(DrawNodePieTest);
    //   ADD_TEST_CASE(DrawNodeVersionsTest);
    ADD_TEST_CASE(DrawNodeFilledPolygonTest);
    ADD_TEST_CASE(DrawNodeDrawInWrongOrder_Issue1888);

    ADD_TEST_CASE(DrawNodeCocos2dxTest1);
    ADD_TEST_CASE(DrawNodeAxmolTest2);
    ADD_TEST_CASE(DrawNodeCocos2dxBackwardsAPITest);
    ADD_TEST_CASE(DrawNodeCocos2dxBetterCircleRendering);
    ADD_TEST_CASE(DrawNodeCocos2dxDrawNodePieTest);
    ADD_TEST_CASE(DrawNodeCocos2dx_Issue829);
    ADD_TEST_CASE(DrawNodeCocos2dx_Issue1319);
}

string DrawNodeExBaseTest::title() const
{
    return "No title";
}

// DrawNodeEx Tests
DrawNodeMorphTest::DrawNodeMorphTest()
{
    auto s = Director::getInstance()->getWinSize();

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);

    segments = 40;

    verticesObj1 = new Vec2[segments];  //circle
    verticesObj2 = new Vec2[segments];  //square
    verticesObjMorph = new Vec2[segments];

    int radius = 100;
    const float coef = 2.0f * (float)M_PI / segments;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    Vec2 center = { s.width / 2, s.height / 2 };
    float angle = 9.0f;
    for (unsigned int i = 0; i < segments; i++) //    //for (int angle = 0; angle < 360; angle += 9) 
    {
        float rads = i * coef;
        verticesObj1[i].x = radius * cosf(rads + angle) * scaleX + center.x;
        verticesObj1[i].y = radius * sinf(rads + angle) * scaleY + center.y;
        verticesObjMorph[i] = verticesObj1[i];
    }

    // A verticesObj2 is a bunch of vertices along straight lines
    int n = 0;
    float delta = segments / 4;
    // Left side of verticesObj2
    for (float y = 50; y > -50; y -= delta) {
        verticesObj2[n++] = Vec2(s.width / 2 - 50, s.height / 2 + y);
    }
    //top
    for (float x = -50; x < 50; x += delta) {
        verticesObj2[n++] = Vec2(s.width / 2 + x, s.height / 2 - 50);
    }
    // Right side
    for (float y = -50; y < 50; y += delta) {
        verticesObj2[n++] = Vec2(s.width / 2 + 50, s.height / 2 + y);
    }
    // Bottom
    for (float x = 50; x > -50; x -= delta) {
        verticesObj2[n++] = Vec2(s.width / 2 + x, s.height / 2 + 50);
    }

    float rad = 80.f;
    for (unsigned int i = 0; i < segments; i++) //    //for (int angle = 0; angle < 360; angle += 9) 
    {
        float rads = i * coef;
        verticesObj1[i].x = radius * cosf(rads + (angle + rad)) * scaleX + center.x;
        verticesObj1[i].y = radius * sinf(rads + (angle + rad)) * scaleY + center.y;
        verticesObjMorph[i] = verticesObj1[i];
    }
    scheduleUpdate();
}
void DrawNodeMorphTest::update(float dt)
{
    auto s = Director::getInstance()->getWinSize();
    static float  rot = 0.1f;
    static float thickness = 0.5f;
    static float deltaThickness = 0.2f;

    drawNodeEx->clear();
    float totalDistance = 0;
    rot += 0;//0.01f;
    thickness += deltaThickness;
    if (thickness > 40 || thickness < 0.5)
    {
        deltaThickness *= -1;
    }

    Vec2 v1, v2;
    for (int i = 0; i < segments; i++) {
        if (state) {
            v1 = verticesObj1[i];
            v2 = verticesObj2[i];
        }
        else {
            v2 = verticesObj1[i];
            v1 = verticesObj2[i];
        }
        v2 = verticesObjMorph[i];
        verticesObjMorph[i] = v2.lerp(v1, 0.05f);
        totalDistance += v1.distance(v2);
    }
    // If all the vertices are close, switch shape
    if (totalDistance < 500.0) {
        state = !state;
    }
    drawNodeEx->setDNRotation(rot);
    drawNodeEx->setDNCenter(Vec2(s.width / 2, s.height / 2));
    drawNodeEx->setIsConvex(true);
    drawNodeEx->drawPolygon(verticesObjMorph, segments, thickness,Color4B::YELLOW);
    drawNodeEx->setIsConvex(true);
}

string DrawNodeMorphTest::title() const
{
    return "Morphing";
}

string DrawNodeMorphTest::subtitle() const
{
    return "";
}

//string DrawNodeExBaseTest::title() const
//{
//    return "No title";
//}

void DrawNodeExBaseTest::drawDirection(const Vec2* vec, const int size, Vec2 offset)
{
    for (size_t i = 0; i < size; i++)
    {
        auto label = Label::createWithTTF(std::to_string(i).c_str(), "fonts/Marker Felt.ttf", 10);
        addChild(label);
        label->setPosition(vec[i] + offset);
    }
}

string DrawNodeFireworkTest::title() const
{
    return "Endless FireWork";
}

string DrawNodeFireworkTest::subtitle() const
{
    return "Performance";
}

DrawNodeFireworkTest::DrawNodeFireworkTest()
{
    auto s = Director::getInstance()->getWinSize();

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);

    projectile = new Vec2[20];

    ember = createFireObjs(30);


    // canon;
    // projectile;
    // wall;

    scheduleUpdate();
}


DrawNodeFireworkTest::fireObj* DrawNodeFireworkTest::createFireObjs(int count)
{
    fireObj* fobj = new fireObj[count];

    for (int n = 0; n < count; n++)
    {
        fobj[n].life = 20 + burnTime * AXRANDOM_0_1();
        fobj[n].x = x + midx;
        fobj[n].y = nomy - y;
        alfa = tupi * AXRANDOM_0_1();
        if (shape > 15)
        {
            fobj[n].vx = sin(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
            fobj[n].vy = cos(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
        }
        else
        {
            fobj[n].vx = sin(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
            fobj[n].vy = cos(alfa) * pow * 0.25;
        }
        if (cmix > 75)
        {
            fobj[n].color = Color4F::WHITE;
        }
        else if (cmix > 65)
        {
            fobj[n].color = Color4F::RED;
        }
        else
        {
            fobj[n].color = Color4F::BLUE;
        }
    }

    return fobj;
}
void DrawNodeFireworkTest::update(float dt)
{
    drawNodeEx->clear();

    float thickness = 2.0f;
    static float rotation = 0.1;
    rotation += 0.1;
    auto s = Director::getInstance()->getWinSize();

    //  drawNodeEx->setRotation(rotation * 3);
    drawNodeEx->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    drawNodeEx->setScale(0.3);
    drawNodeEx->setPosition({ 200,80 });

    // drawNodeEx->setPosition(0.1);
    Vec2 gear1 = { 280.f, 320.f };
    Vec2 gear2 = { 160.f, 320.f };
    Vec2 gear3 = { 200.f, 200.f };
    Vec2 gear4 = { s.width - 200, s.height - 200 };

    drawNodeEx->drawLine(gear2, gear4, Color4F::RED, thickness);  // line
    //  DrawNodeEx::DNObject* test1 = drawNodeEx->getDNObject();
    drawNodeEx->setDNCenter(gear1);
    drawNodeEx->setDNRotation(rotation + 45);
    drawNodeEx->drawStar(Vec2(gear1), 30, 60, 8, Color4F::BLUE, 4.0);
    drawNodeEx->setDNRotation(-rotation);
    drawNodeEx->setDNCenter(gear2);
    drawNodeEx->drawSolidStar(gear2, 30, 60, 8, Color4F::GREEN, Color4F::YELLOW, 4.0);
    //   drawDirection(gear2, 30, gear2[0]->getPosition());

    drawNodeEx->resetDNValues();
    drawNodeEx->drawLine(gear2, gear1, Color4F::RED, thickness);  // line
    drawNodeEx->setDNCenter(gear4);
    drawNodeEx->setDNRotation(rotation + 45);
    drawNodeEx->drawStar(gear3, 30, 60, 18, Color4F::RED, 1.0);
    drawNodeEx->drawLine(gear3, gear4, Color4F::YELLOW, thickness);  // line
    drawNodeEx->resetDNValues();
    drawNodeEx->setDNRotation(rotation - 45);
    drawNodeEx->setDNCenter(gear4);
    drawNodeEx->drawStar(gear4, 40, 60, 60, Color4F::GREEN, 1.0);
    drawNodeEx->resetDNValues();

    //  DrawNodeEx::DNObject* test = drawNodeEx->getDNObject();
    drawNodeEx->setDNScale(Vec2(0.5f, 0.5f));
    drawNodeEx->setDNPosition(Vec2(100, 100));
    //drawNodeEx->drawPoly(test->_vertices, test->_size, true, Color4B::ORANGE);
    //drawNodeEx->drawPoly(test1->_vertices, test1->_size, true, Color4B::ORANGE);
    drawNodeEx->resetDNValues();


    // drawRect
    drawNodeEx->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1), thickness);
    drawNodeEx->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1), thickness);
    drawNodeEx->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F(1, 1, 0, 1), thickness);
    drawNodeEx->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::MAGENTA, thickness);

    // drawCircle
    drawNodeEx->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
        Color4F(1.0f, 0.0f, 0.0f, 0.5f), thickness);
    drawNodeEx->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, true,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    // drawCubicBezier
    drawNodeEx->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
        Vec2(s.width - 10, s.height - 10), 10, Color4F::BLUE, thickness);
    drawNodeEx->drawQuadBezier(Vec2(0.0f + 100, s.height - 100), Vec2(s.width / 2, s.height / 2),
        Vec2(s.width - 100, s.height - 100), 50, Color4F::RED, thickness);

    // drawCubicBezier
    drawNodeEx->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
        Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(),
        100, Color4F::WHITE, thickness);
    drawNodeEx->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
        Vec2(s.width - 10, s.height - 50), 10, Color4F::GRAY, thickness);

    // drawCardinalSpline
    auto array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    drawNodeEx->drawCardinalSpline(array, 0.5f, 50, Color4F::MAGENTA, thickness);

    auto array2 = ax::PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 80.0f));
    array2->addControlPoint(Vec2(s.width - 80, 80.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 80.0f));
    drawNodeEx->drawCardinalSpline(array2, 5.0f, 50, Color4F::ORANGE, thickness);

    // drawCatmullRom
    array2 = ax::PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 80.0f));
    array2->addControlPoint(Vec2(s.width - 80, 80.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 80.0f));
    drawNodeEx->drawCatmullRom(array2, 50, Color4F::ORANGE, thickness);

    array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    drawNodeEx->drawCatmullRom(array, 50, Color4F::MAGENTA, thickness);

    // drawPoly
    // for (int n = 0; n < 10; n++)
    bool isReal = false;

    // drawNodeEx->drawPoly(verticess, sizeof(verticess) / sizeof(verticess[0]), true,
    //                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    Vec2 vertices[5] = { Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
        Vec2(50.0f, 100.0f) };
    drawNodeEx->drawPoly(vertices, 5, false, Color4B::BLUE, thickness);

    Vec2 vertices2[3] = { Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f) };
    drawNodeEx->drawPoly(vertices2, 3, true, Color4B::GREEN, thickness);

    drawNodeEx->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4B::RED, thickness);

    // drawPolygon

    drawNodeEx->setDNScale(Vec2(thickness, thickness));
    drawNodeEx->setDNPosition(Vec2(0, 0));
    drawNodeEx->setDNRotation(0);
    drawNodeEx->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
        Color4F::YELLOW);
    drawNodeEx->setDNPosition(Vec2(0, 0));
    drawNodeEx->setDNRotation(thickness);
    drawNodeEx->setDNScale(Vec2(thickness, thickness));
    drawNodeEx->setDNCenter(vertices1[0]);
    drawNodeEx->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
        Color4F::YELLOW);










    //if (fuse > 0)
    //{
    //    drawNodeEx->drawLine(Vec2(ember[a].x, ember[a].y),
    //        Vec2((ember[a].x - ember[a].vx), (ember[a].y - ember[a].vy)), Color4B::GREEN); //[a].color);
    //    fuse--;
    //    a = old - 1;
    //    do
    //    {
    //        a++;
    //        if (a > max)
    //        {
    //            a = 0;
    //        }
    //        ember[a].life--;
    //        if (ember[a].life <= 0)
    //        {
    //            old++;
    //            if (old > max)
    //                old = 0;
    //        }
    //        else
    //        {
    //            AXLOG("%f %f", ember[a].x, ember[a].y);

    //            ember[a].x += ember[a].vx;
    //            ember[a].y += ember[a].vy;
    //            ember[a].vx *= drag;
    //            ember[a].vy *= drag + gravity;
    //        }

    //    } while (a != young);
    //}
    //else  // fuse has reached the end so create a new burst
    //{
    //    x = 300;
    //    y = 300;
    //    emberCount = 50 + 100 * AXRANDOM_0_1();
    //    pow = emberCount / impulse;

    //    cmix = 100 * AXRANDOM_0_1();
    //    c1 = 5 * AXRANDOM_0_1();
    //    c2 = 5 * AXRANDOM_0_1();
    //    while (c1 == c2)
    //    {
    //        c2 = 5 * AXRANDOM_0_1();
    //    };
    //    shape = 100 * AXRANDOM_0_1();
    //    a = young;
    //    for (int n = 0; n < emberCount; n++)
    //    {
    //        ember[n].life = 20 + burnTime * AXRANDOM_0_1();
    //        ember[n].x = x + midx;
    //        ember[n].y = nomy - y;
    //        alfa = tupi * AXRANDOM_0_1();
    //        if (shape > 15)
    //        {
    //            ember[n].vx = sin(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
    //            ember[n].vy = cos(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
    //        }
    //        else
    //        {
    //            ember[n].vx = sin(alfa) * pow * sin(AXRANDOM_0_1() * tupi);
    //            ember[n].vy = cos(alfa) * pow * 0.25;
    //        }
    //        if (cmix > 75)
    //        {
    //            ember[n].color = Color4F::WHITE;
    //        }
    //        else if (cmix > 65)
    //        {
    //            ember[n].color = Color4F::RED;
    //        }
    //        else
    //        {
    //            ember[n].color = Color4F::BLUE;
    //        }
    //    }
    //    young = a;
    //    fuse = 25;    // AXRANDOM_0_1() * fuseTime + 25;
    //} 
}

// DrawNodeEx Tests
DrawNodePictureTest::DrawNodePictureTest()
{
    auto s = Director::getInstance()->getWinSize();

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);

    scheduleUpdate();
}
void DrawNodePictureTest::update(float dt)
{
    static float rot = 0.1f;
    static int count = 0;
    static bool wait = false;

    drawNodeEx->clear();

    if (!wait)
    {
        rot += 0.05;
        if (rot >= 6)
        {
            rot = count = 0;
            wait = true;
        }
    }
    else if (count++ > 30)
        wait = false;

    float sph_xx[2326];
    float sph_yy[2326];
    int n = 0;
    for (int i = 0; i < 2326;)  // read data
    {
        sph_xx[n] = verticesHead[i++];
        sph_yy[n] = verticesHead[i++];
        n++;
    }

    float sph_cmb = sph_yy[0];
    int sph_la = 0;
    do
    {
        Color4F color = Color4F(sph_xx[sph_la + 1], sph_yy[sph_la + 1], sph_xx[sph_la + 2], sph_yy[sph_la + 2] * 255);
        // color = Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1);
        Vec2* vertices = new Vec2[(int)(sph_cmb - 3)];
        for (int n = 3; n < sph_cmb; n++)
        {
            vertices[n - 3] = Vec2(sph_xx[sph_la + n], sph_yy[sph_la + n]);
        }
        drawNodeEx->setPosition(Vec2(420, 280));
        drawNodeEx->setScale(0.4);
        drawNodeEx->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        drawNodeEx->setRotation(180);
        drawNodeEx->setDNCenter(vertices[0]);
        drawNodeEx->setDNRotation(rot);
        drawNodeEx->setIsConvex(true);
        drawNodeEx->drawPolygon(vertices, sph_cmb - 3, color, /*rot*/0.f, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));
        drawNodeEx->setIsConvex(false);

        sph_la += sph_cmb;
        sph_cmb = sph_yy[sph_la];
    } while (sph_yy[sph_la] != 0);
}

string DrawNodePictureTest::title() const
{
    return "Picture";
}

string DrawNodePictureTest::subtitle() const
{
    return "";//"Rotation, Filled Polygon, Individual Thickness ";
}

// DrawNodeCocos2dxTest1
DrawNodeCocos2dxTest1::DrawNodeCocos2dxTest1()
{
    auto s = Director::getInstance()->getWinSize();

    auto draw = ax::extension::DrawNodeEx::create();
    addChild(draw, 10);

    draw->drawPoint(Vec2(s.width / 2 - 120, s.height / 2 - 120), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    draw->drawPoint(Vec2(s.width / 2 + 120, s.height / 2 + 120), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw 4 small points
    Vec2 position[] = { Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60) };
    draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a line
    draw->drawLine(Vec2(0, 0), Vec2(s.width, s.height), Color4F(1.0, 0.0, 0.0, 0.5));

    // draw a rectangle
    draw->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1));

    draw->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a circle
    draw->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
        Color4F(1.0f, 0.0f, 0.0f, 0.5f));

    draw->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw some beziers
    draw->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
        Vec2(s.width - 10, s.height - 10), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawQuadBezier(Vec2(0.0f, s.height), Vec2(s.width / 2, s.height / 2), Vec2(s.width, s.height), 50,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
        Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(), 100,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
        Vec2(s.width - 10, s.height - 50), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    draw->drawCardinalSpline(array, 0.5f, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array2 = ax::PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    draw->drawCatmullRom(array2, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    // open random color poly
    Vec2 vertices[] = { Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                       Vec2(50.0f, 100.0f) };
    draw->drawPoly(vertices, 5, false, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // closed random color poly
    Vec2 vertices2[] = { Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f) };
    draw->drawPoly(vertices2, 3, true, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw 10 circles
    for (int i = 0; i < 10; i++)
    {
        draw->drawDot(Vec2(s.width / 2, s.height / 2), 10 * (10 - i),
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    // Draw polygons
    Vec2 points[] = { Vec2(s.height / 4, 0.0f), Vec2(s.width, s.height / 5), Vec2(s.width / 3 * 2, s.height) };
    draw->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 4,
        Color4F(0.0f, 0.0f, 1.0f, 0.5f));

    // star poly (triggers buggs)
    {
        const float o = 80;
        const float w = 20;
        const float h = 50;
        Vec2 star[] = {
            Vec2(o + w, o - h), Vec2(o + w * 2, o),                  // lower spike
            Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2),  // right spike
            //              {o +w, o+w*2+h}, {o,o+w*2},                 // top spike
            //              {o -h, o+w}, {o,o},                         // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
            Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // star poly (doesn't trigger bug... order is important un tesselation is supported.
    {
        const float o = 180;
        const float w = 20;
        const float h = 50;
        Vec2 star[] = {
            Vec2(o, o),
            Vec2(o + w, o - h),
            Vec2(o + w * 2, o),  // lower spike
            Vec2(o + w * 2 + h, o + w),
            Vec2(o + w * 2, o + w * 2),  // right spike
            Vec2(o + w, o + w * 2 + h),
            Vec2(o, o + w * 2),  // top spike
            Vec2(o - h, o + w),  // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
            Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // draw a solid polygon
    Vec2 vertices3[] = { Vec2(60.0f, 160.0f), Vec2(70.0f, 190.0f), Vec2(100.0f, 190.0f), Vec2(90.0f, 160.0f) };
    draw->drawSolidPoly(vertices3, 4, Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid rectangle
    draw->drawSolidRect(Vec2(10.0f, 10.0f), Vec2(20.0f, 20.0f), Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid circle
    draw->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
        Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    // Draw segment
    draw->drawSegment(Vec2(20.0f, s.height), Vec2(20.0f, s.height / 2), 10, Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    draw->drawSegment(Vec2(10.0f, s.height / 2), Vec2(s.width / 2, s.height / 2), 40, Color4F(1.0f, 0.0f, 1.0f, 0.5f));

    // Draw triangle
    draw->drawTriangle(Vec2(10.0f, 10.0f), Vec2(70.0f, 30.0f), Vec2(100.0f, 140.0f),
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    for (int i = 0; i < 100; i++)
    {
        draw->drawPoint(Vec2(i * 7.0f, 5.0f), (float)i / 5 + 1,
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    auto draw1 = ax::extension::DrawNodeEx::create();
    this->addChild(draw1, 10);
    draw1->setLineWidth(4);
    draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    draw->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
    draw1->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2), FadeOut::create(1.2), NULL)));
}

string DrawNodeCocos2dxTest1::title() const
{
    return "Cocos2dx DrawNode test";
}

string DrawNodeCocos2dxTest1::subtitle() const
{
    return "";
}

//
//
//
DrawNodeThicknessTest::DrawNodeThicknessTest()
{
    // Add lines to see the correct "scale of the 'rings'" changing the window size

    auto draw = DrawNodeEx::create();
    draw->setLineWidth(1);
    addChild(draw, 10);

    for (float y = 0; y < VisibleRect::top().y; y += 10)
    {
        draw->drawLine({ VisibleRect::left().x, y }, { VisibleRect::right().x, y }, Color4B::GRAY);
    }
    initSliders();

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx, 10);

    auto s = Director::getInstance()->getWinSize();
    // auto draw1 = DrawNodeExt::create();
    // this->addChild(draw1, 10);
    //// draw1->setLineWidth(4);
    // draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    // draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    scheduleUpdate();
}

void DrawNodeThicknessTest::changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderthickness = dynamic_cast<ax::ui::Slider*>(pSender);
        thickness = static_cast<float>(sliderthickness->getPercent()) / 10.0f;
        _thicknessLabel->setString("Thickness " + Value(thickness).asString());
    }
}

void DrawNodeThicknessTest::initSliders()
{

    // Layer => LayerRadialGradientTest   ############################################# Peter Eismann

    auto vsize = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* slider = ax::ui::Slider::create();
    slider->setPercent(0);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->setPosition(Vec2(vsize.width / 2, vsize.height / 6));

    slider->addEventListener(AX_CALLBACK_2(DrawNodeThicknessTest::changeThickness, this));

    auto ttfConfig = TTFConfig("fonts/arial.ttf", 8);
    _thicknessLabel = Label::createWithTTF(ttfConfig, "Thickness ");
    addChild(_thicknessLabel, 20);
    _thicknessLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 15));

    addChild(slider, 20);
}

void DrawNodeThicknessTest::update(float dt)
{
    auto s = Director::getInstance()->getWinSize();

    drawNodeEx->clear();
    auto color = Color4F::GREEN;
    drawNodeEx->drawCircle(VisibleRect::center() /*- Vec2(120.0f, 0.0f)*/, 60, AX_DEGREES_TO_RADIANS(77), 36, false,
        color, thickness);

    // drawNodeEx->drawCircle(VisibleRect::center() /*- Vec2(120.0f, 0.0f)*/, 60, AX_DEGREES_TO_RADIANS(77), 36, false,
    //                      Color4F::BLACK, 1);

    // drawNodeEx->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW, thickness);
    // drawNodeEx->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW, thickness);

    // draw a rectangles
    drawNodeEx->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F(1, 1, 0, 1), thickness);
    // drawNodeEx->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F::BLACK, 1);

    drawNodeEx->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::MAGENTA, thickness);
    // drawNodeEx->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::BLACK, 1);

    drawNodeEx->drawLine(Vec2(200.0f, s.height - 20), Vec2(s.width - 100, s.height - 20), Color4F::YELLOW, thickness);
    drawNodeEx->drawLine(Vec2(300.0f, 100.0f), Vec2(s.width - 200, s.height - 120), Color4F::GREEN, thickness);

    Vec2 vertices24[] = {
        {45.750000f, 144.375000f},  {75.500000f, 136.875000f},  {75.500000f, 159.125000f},  {100.250000f, 161.375000f},
        {65.500000f, 181.375000f},  {102.250000f, 179.125000f}, {95.000000f, 215.125000f},  {129.331467f, 189.926208f},
        {131.371460f, 206.366196f}, {139.651474f, 192.446198f}, {161.851471f, 200.606201f}, {151.000000f, 220.375000f},
        {110.500000f, 244.375000f}, {153.750000f, 238.125000f}, {142.500000f, 253.875000f}, {220.750000f, 259.375000f},
        {250.500000f, 244.375000f}, {168.750000f, 241.875000f}, {182.250000f, 154.125000f}, {190.250000f, 227.375000f},
        {196.500000f, 197.375000f}, {208.750000f, 210.625000f}, {220.750000f, 194.375000f}, {208.750000f, 176.375000f},
        {253.250000f, 173.875000f}, {243.750000f, 154.125000f}, {213.750000f, 161.375000f}, {202.250000f, 139.875000f},
        {236.000000f, 131.875000f}, {218.500000f, 120.875000f}, {206.500000f, 125.625000f}, {184.500000f, 110.375000f},
        {157.000000f, 108.625000f}, {147.500000f, 96.625000f},  {153.750000f, 85.125000f},  {147.500000f, 75.375000f},
        {126.500000f, 74.125000f},  {110.500000f, 86.625000f},  {127.750000f, 85.125000f},  {135.250000f, 91.125000f},
        {135.250000f, 97.875000f},  {124.000000f, 93.875000f},  {115.500000f, 100.875000f}, {115.500000f, 111.875000f},
        {135.250000f, 108.625000f}, {151.000000f, 124.125000f}, {90.500000f, 131.875000f},  {113.250000f, 120.875000f},
        {88.000000f, 116.875000f},  {106.000000f, 103.875000f}, {88.000000f, 97.875000f},
    };
    drawNodeEx->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4B::TRANSPARENT, thickness,
        Color4F::RED);
    // drawNodeEx->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4B::TRANSPARENT, 0.5f,
    //                       Color4F::BLACK);

    // open random color poly
    Vec2 vertices[] = { Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                       Vec2(50.0f, 100.0f) };
    drawNodeEx->drawPoly(vertices, 5, false, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    // closed random color poly
    Vec2 vertices2[] = { Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f) };
    drawNodeEx->drawPoly(vertices2, 3, true, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

    // Draw some beziers
    // Draw some beziers
    drawNodeEx->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
        Vec2(s.width - 10, s.height - 10), 10, Color4F::BLUE, thickness);

    drawNodeEx->drawQuadBezier(Vec2(0.0f + 100, s.height - 100), Vec2(s.width / 2, s.height / 2),
        Vec2(s.width - 100, s.height - 100), 50, Color4F::RED, thickness);

    drawNodeEx->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
        Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(),
        100, Color4F::WHITE, thickness);

    drawNodeEx->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
        Vec2(s.width - 10, s.height - 50), 10, Color4F::GRAY, thickness);

    auto array = ax::PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    drawNodeEx->drawCardinalSpline(array, 0.5f, 50, Color4F::MAGENTA, thickness);

    auto array2 = ax::PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    drawNodeEx->drawCatmullRom(array2, 50, Color4F::ORANGE, thickness);
}

string DrawNodeThicknessTest::title() const
{
    return "Thickness Test";
}

string DrawNodeThicknessTest::subtitle() const
{
    return "";
}

DrawNodePieTest::DrawNodePieTest()
{
    drawNode = DrawNodeEx::create();
    addChild(drawNode, 10);

    initSliders();

    scheduleUpdate();
}

void DrawNodePieTest::changeEndAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sEndAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        endAngle = sEndAngle->getPercent() * 3.6;
        _EndAngleLabel->setString("endAngle: (" + Value(endAngle).asString() + ")");
    }
}

void DrawNodePieTest::changeStartAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        startAngle = sStartAngle->getPercent() * 3.6;
        _StartAngleLabel->setString("startAngle: (" + Value(startAngle).asString() + ")");
    }
}

void DrawNodePieTest::changeRotation(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        rotation = sStartAngle->getPercent() * 3.6;
        _RotationLabel->setString("Rotation: (" + Value(rotation).asString() + ")");
    }
}

void DrawNodePieTest::changeThickness(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        thickness = sStartAngle->getPercent() * 0.1;
        _ThicknessLabel->setString("Thickness: (" + Value(thickness).asString() + ")");
    }
}

void DrawNodePieTest::initSliders()
{
    rotation = 324;
    endAngle = 30;
    startAngle = 0;
    thickness = 1;

    auto vsize = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* sliderStartAngle = ax::ui::Slider::create();
    sliderStartAngle->setPercent(startAngle);
    sliderStartAngle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderStartAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderStartAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderStartAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderStartAngle->setPosition(Vec2(20, vsize.height / 6));

    sliderStartAngle->addEventListener(AX_CALLBACK_2(DrawNodePieTest::changeStartAngle, this));

    auto ttfConfig = TTFConfig("fonts/arial.ttf", 8);
    _StartAngleLabel = Label::createWithTTF(ttfConfig, "StartAngle (" + Value(startAngle).asString() + ")");
    _StartAngleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_StartAngleLabel, 20);
    _StartAngleLabel->setPosition(sliderStartAngle->getPosition() + Vec2(sliderStartAngle->getContentSize().x + 20, 0));

    addChild(sliderStartAngle, 20);

    ax::ui::Slider* sliderEndAngle = ax::ui::Slider::create();
    sliderEndAngle->setPercent(endAngle);
    sliderEndAngle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderEndAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderEndAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderEndAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderEndAngle->setPosition(Vec2(20, vsize.height / 6 + 25));
    sliderEndAngle->addEventListener(AX_CALLBACK_2(DrawNodePieTest::changeEndAngle, this));

    _EndAngleLabel = Label::createWithTTF(ttfConfig, "endAngle (" + Value(endAngle).asString() + ")");
    _EndAngleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_EndAngleLabel, 20);
    _EndAngleLabel->setPosition(sliderEndAngle->getPosition() + Vec2(sliderEndAngle->getContentSize().x + 20, 0));
    addChild(sliderEndAngle, 20);

    ax::ui::Slider* sliderRotation = ax::ui::Slider::create();
    sliderRotation->setPercent(rotation);
    sliderRotation->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderRotation->loadBarTexture("cocosui/sliderTrack.png");
    sliderRotation->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderRotation->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderRotation->setPosition(Vec2(20, vsize.height / 6 + 50));
    sliderRotation->addEventListener(AX_CALLBACK_2(DrawNodePieTest::changeRotation, this));

    _RotationLabel = Label::createWithTTF(ttfConfig, "Rotation (" + Value(rotation).asString() + ")");
    _RotationLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_RotationLabel, 20);
    _RotationLabel->setPosition(sliderRotation->getPosition() + Vec2(sliderRotation->getContentSize().x + 20, 0));
    addChild(sliderRotation, 20);

    ax::ui::Slider* sliderThickness = ax::ui::Slider::create();
    sliderThickness->setPercent(thickness);
    sliderThickness->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    sliderThickness->loadBarTexture("cocosui/sliderTrack.png");
    sliderThickness->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderThickness->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderThickness->setPosition(Vec2(20, vsize.height / 6 + 75));
    sliderThickness->addEventListener(AX_CALLBACK_2(DrawNodePieTest::changeThickness, this));

    _ThicknessLabel = Label::createWithTTF(ttfConfig, "Thickness (" + Value(thickness).asString() + ")");
    _ThicknessLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    addChild(_ThicknessLabel, 20);
    _ThicknessLabel->setPosition(sliderThickness->getPosition() + Vec2(sliderThickness->getContentSize().x + 20, 0));
    addChild(sliderThickness, 20);
}

void DrawNodePieTest::update(float dt)
{
    drawNode->clear();
    drawNode->drawPie(VisibleRect::center() - Vec2(170.0f, -35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Fill, thickness);
    drawNode->drawPie(VisibleRect::center() - Vec2(60.0f, -35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::TRANSPARENT, Color4F::BLUE, drawNode->DrawMode::Outline, thickness);
    drawNode->drawPie(VisibleRect::center() + Vec2(60.0f, 35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Line, thickness);
    drawNode->drawPie(VisibleRect::center() + Vec2(170.0f, 35.0f), 50, rotation, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::RED, Color4F::BLUE, drawNode->DrawMode::Semi, thickness);
}

string DrawNodePieTest::title() const
{
    return "drawPie";
}

string DrawNodePieTest::subtitle() const
{
    return "Filled, Outlined, Line, Semi";
}

DrawNodeVersionsTest::DrawNodeVersionsTest()
{
    auto director = Director::getInstance();
    // director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size = director->getVisibleSize();
    center = Vec2(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    float o = 80;
    float w = 20;
    float h = 50;
    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);
    //  drawNodeEx->setPosition(center);

    {  // star

        Vec2 star[] = {
            Vec2(o, o),
            Vec2(o + w, o - h),
            Vec2(o + w * 2, o),  // lower spike
            Vec2(o + w * 2 + h, o + w),
            Vec2(o + w * 2, o + w * 2),  // right spike
            Vec2(o + w, o + w * 2 + h),
            Vec2(o, o + w * 2),  // top spike
            Vec2(o - h, o + w),  // left spike
        };

        drawNodeEx->setDNPosition(center);
        drawNodeEx->setDNCenter(star[0]);
        //   drawNodeEx->setDNRotation(rot);
        drawNodeEx->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
            Color4F::BLUE);
    }

    scheduleUpdate();
}

void DrawNodeVersionsTest::drawDirection(const Vec2* vec, const int size, Vec2 offset)
{
    for (size_t i = 0; i < size; i++)
    {
        auto label = Label::createWithTTF(std::to_string(i).c_str(), "fonts/Marker Felt.ttf", 10);
        this->addChild(label);
        label->setPosition(vec[i] + offset);
    }
}

string DrawNodeVersionsTest::title() const
{
    return "DrawNodeVersion and Rotation";
}

string DrawNodeVersionsTest::subtitle() const
{
    return "YELLOW=v1 ORANGE=v2";
}

void DrawNodeVersionsTest::update(float dt)
{
    float rot = 0.1;
    float o = 80;
    float w = 20;
    float h = 50;
    {  // star

        Vec2 star[] = {
            Vec2(o, o),
            Vec2(o + w, o - h),
            Vec2(o + w * 2, o),  // lower spike
            Vec2(o + w * 2 + h, o + w),
            Vec2(o + w * 2, o + w * 2),  // right spike
            Vec2(o + w, o + w * 2 + h),
            Vec2(o, o + w * 2),  // top spike
            Vec2(o - h, o + w),  // left spike
        };

        /*        drawNodeEx->setDNPosition(center);
                drawNodeEx->setDNCenter(star[0]);
                drawNodeEx->setDNRotation(rot)*/;
                drawNodeEx->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
                    Color4F::BLUE);
    }
    {
        int x = 0;
        int y = 0;
        Vec2 vertices[4];
        drawNodeEx->setScale(0.5);
        Color4F color;
        for (int iy = 0; iy < 5; iy++)
        {
            x = 0;
            for (int ix = 0; ix < 13; ix++)
            {
                vertices[0] = Vec2(x + o + w, y + o - h);
                vertices[1] = Vec2(x + o + w * 2, y + o);
                vertices[2] = Vec2(x + o + w * 2 + h, y + o + w);
                vertices[3] = Vec2(x + o + w * 2, y + o + w * 2);

                if (AXRANDOM_0_1() > 0.5f)
                {
                    drawNodeEx->setIsConvex(true);
                    color = Color4F::YELLOW;
                }
                else
                {
                    drawNodeEx->setIsConvex(false);
                    color = Color4F::ORANGE;
                }
                //drawNodeEx->setDNRotation(rot);
                //drawNodeEx->setDNCenter(vertices[0]);
                //drawNodeEx->setDNPosition(Vec2(-70.f, 60.f));
                drawNodeEx->drawPolygon(vertices, 4, Color4F(0.7f, 0.7f, 0.7f, 0.5f), 1, color);
                drawNodeEx->setIsConvex(false);
                x += 70;
            }
            y += 80;
        }
    }

    rot += 0.1;
}

DrawNodeFilledPolygonTest::DrawNodeFilledPolygonTest()
{

    DrawNodeEx* drawNode[3];
    for (int i = 0; i < 3; i++)
    {
        drawNode[i] = DrawNodeEx::create();
        addChild(drawNode[i]);
    }

    // draw a solid circle
    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f,
        2.0f, Color4F::BLUE);

    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(-40.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f,
        2.0f, Color4F::WHITE);

    drawNode[0]->setPosition(Vec2(-30, -20));
    // drawNodeEx[0]->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4F::RED, 0.3f,
    //                          Color4F::GREEN);

    drawNode[2]->drawPolygon(vertices2, sizeof(vertices2) / sizeof(vertices2[0]), Color4F::GREEN, 0.3f,
        Color4F::YELLOW);

    drawNode[2]->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::TRANSPARENT, 5.3f,
        Color4F::RED);
}

string DrawNodeFilledPolygonTest::title() const
{
    return "Filled Polygon Test #2";
}

string DrawNodeFilledPolygonTest::subtitle() const
{
    return "";
}

// DrawMethodesThicknessTest
DrawNodeMethodesTest::DrawNodeMethodesTest()
{
    _currentSeletedItemIndex = 0;

    auto director = Director::getInstance();
    director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size = director->getVisibleSize();
    Vec2 center(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    auto listview = createListView();
    listview->setPosition(Vec2(0.0f, 40.0f));
    addChild(listview);

    draw = DrawNodeEx::create();
    draw->setScale(0.5);
    draw->setPosition(center);
    addChild(draw);

    draw1 = DrawNodeEx::create();
    draw1->setScale(0.5);
    draw1->setPosition(size / 4);
    addChild(draw1);

    auto thicknessSlider = createSlider();
    thicknessSlider->setPosition(Vec2(center.x, 60.0f));
    addChild(thicknessSlider);

    label = Label::createWithTTF("Value: ", "fonts/Marker Felt.ttf", 10);
    label->setPosition(Vec2(Vec2(center.x, 80.0f)));
    this->addChild(label, 1);

    label1 = Label::createWithTTF("DrawNodeEx::Round", "fonts/Arial.ttf", 12);
    addChild(label1, 1);
    label2 = Label::createWithTTF("DrawNodeEx::Square", "fonts/Arial.ttf", 12);
    addChild(label2, 1);
    label3 = Label::createWithTTF("DrawNodeEx::Butt", "fonts/Arial.ttf", 12);
    addChild(label3, 1);

    isDirty = true;
    scheduleUpdate();
}

std::string DrawNodeMethodesTest::title() const
{
    return "Draw Methods";
}

string DrawNodeMethodesTest::subtitle() const
{
    return "";
}

ax::ui::Slider* DrawNodeMethodesTest::createSlider()
{
    auto slider = ui::Slider::create();
    slider->setTouchEnabled(true);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->addEventListener(AX_CALLBACK_2(DrawNodeMethodesTest::sliderCallback, this));
    slider->setTag(101);
    slider->setPercent(10);

    return slider;
}

void DrawNodeMethodesTest::listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type)
{
    // clear all text to white
    auto listview = static_cast<ax::ui::ListView*>(sender);
    for (auto&& item : listview->getItems())
    {
        static_cast<ax::ui::Text*>(item)->setColor(ax::Color3B::WHITE);
    }
    _currentSeletedItemIndex = (int)listview->getCurSelectedIndex();
    listview->getItem(_currentSeletedItemIndex)->setColor(ax::Color3B::RED);
    isDirty = true;
}

void DrawNodeMethodesTest::sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = dynamic_cast<ax::ui::Slider*>(sender);
        thickness = slider->getPercent() * 0.1f;
        count = slider->getPercent() * 10;
    }
    isDirty = true;
}

ax::ui::ListView* DrawNodeMethodesTest::createListView()
{
    auto listview = ax::ui::ListView::create();
    Vec2 contentSize = { 0,0 };
    for (size_t i = 0; i < drawMethodsCounter; i++)
    {
        auto ui = ax::ui::Text::create();
        ui->setString(drawMethods[i].c_str());
        contentSize.x = MAX(ui->getContentSize().x, contentSize.x);
        contentSize.y = MAX(ui->getContentSize().y, contentSize.y);
        ui->setTouchEnabled(true);
        listview->pushBackCustomItem(ui);
    }

    listview->setContentSize(contentSize * drawMethodsCounter);
    listview->setCurSelectedIndex(0);
    listview->setTouchEnabled(true);
    listview->addEventListener(
        (ui::ListView::ccListViewCallback)AX_CALLBACK_2(DrawNodeMethodesTest::listviewCallback, this));
    listview->setTag(100);

    listview->getItem(_currentSeletedItemIndex)->setColor(Color3B::RED);

    return listview;
}

void DrawNodeMethodesTest::update(float dt)
{
    // if (isDirty== true)
    {
        drawAll();
        isDirty = false;
    }
}

void DrawNodeMethodesTest::drawAll()
{
    isDirty = false;
    static float rotation = 0.1;
    rotation += 0.1;
    auto s = Director::getInstance()->getWinSize();

    draw->clear();
    label->setString("Thickness: (" + Value(thickness).asString() + ")");

    switch (_currentSeletedItemIndex)
    {
    case 0:
    {
        // drawLine
        //float nodeRotation = draw->getRotation();
        //draw->setRotation(rotation * 3);
      //  draw->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        draw->setScale(0.3);

        // draw->setPosition(0.1);
        Vec2 gear1 = { 280.f, 320.f };
        Vec2 gear2 = { 160.f, 320.f };
        Vec2 gear3 = { 200.f, 200.f };
        Vec2 gear4 = { s.width - 200, s.height - 200 };

        draw->drawLine(gear2, gear4, Color4F::BLUE, thickness);  // line
        //draw->setDNCenter(gear1);
        //draw->setDNRotation(rotation + 45);
        //draw->drawStar(Vec2(gear1), 30, 60, 8, Color4F::BLUE, 4.0);
        //draw->setDNRotation(-rotation);
        //draw->setDNCenter(gear2);
        //draw->drawSolidStar(gear2, 30, 60, 8, Color4F::GREEN, Color4F::YELLOW, 4.0);

        //draw->resetDNValues();
        draw->drawLine(gear2, gear1, Color4F::RED, thickness);  // line
 /*       draw->setDNCenter(gear4);
        draw->setDNRotation(rotation + 45);
        draw->drawStar(gear3, 30, 60, 18, Color4F::RED, 1.0);*/
        draw->drawLine(gear3, gear4, Color4F::YELLOW, thickness);  // line
        //draw->resetDNValues();
        //draw->setDNRotation(rotation - 45);
        //draw->setDNCenter(gear4);
        //draw->drawStar(gear4, 40, 60, 60, Color4F::GREEN, 1.0);
        //draw->resetDNValues();
  //      isDirty = true;
 //       draw->setRotation(nodeRotation);
        break;
    }
    case 1:
    {
        // drawRect
        draw->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1), thickness);
        draw->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1), thickness);
        draw->drawRect(Vec2(123, 123), Vec2(227, 227), Color4F(1, 1, 0, 1), thickness);
        draw->drawRect(Vec2(115, 130), Vec2(130, 115), Vec2(115, 100), Vec2(100, 115), Color4F::MAGENTA, thickness);
        break;
    }
    case 2:
    {
        // drawCircle
        draw->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
            Color4F(1.0f, 0.0f, 0.0f, 0.5f), thickness);
        draw->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, true,
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);
        break;
    }
    case 3:
    {
        // drawCubicBezier
        draw->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
            Vec2(s.width - 10, s.height - 10), 10, Color4F::BLUE, thickness);
        draw->drawQuadBezier(Vec2(0.0f + 100, s.height - 100), Vec2(s.width / 2, s.height / 2),
            Vec2(s.width - 100, s.height - 100), 50, Color4F::RED, thickness);
        break;
    }
    case 4:
    {
        // drawCubicBezier
        draw->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
            Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(),
            100, Color4F::WHITE, thickness);
        draw->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
            Vec2(s.width - 10, s.height - 50), 10, Color4F::GRAY, thickness);
        break;
    }
    case 5:
    {
        // drawCardinalSpline
        auto array = ax::PointArray::create(20);
        array->addControlPoint(Vec2(0.0f, 0.0f));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array->addControlPoint(Vec2(80.0f, s.height - 80));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width / 2, s.height / 2));
        draw->drawCardinalSpline(array, 0.5f, 50, Color4F::MAGENTA, thickness);

        auto array2 = ax::PointArray::create(20);
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        draw->drawCardinalSpline(array2, 5.0f, 50, Color4F::ORANGE, thickness);
        break;
    }
    case 6:
    {
        // drawCatmullRom
        auto array2 = ax::PointArray::create(20);
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, 80.0f));
        array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
        array2->addControlPoint(Vec2(s.width / 2, 80.0f));
        draw->drawCatmullRom(array2, 50, Color4F::ORANGE, thickness);

        auto array = ax::PointArray::create(20);
        array->addControlPoint(Vec2(0.0f, 0.0f));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, 80.0f));
        array->addControlPoint(Vec2(s.width - 80, s.height - 80));
        array->addControlPoint(Vec2(80.0f, s.height - 80));
        array->addControlPoint(Vec2(80.0f, 80.0f));
        array->addControlPoint(Vec2(s.width / 2, s.height / 2));
        draw->drawCatmullRom(array, 50, Color4F::MAGENTA, thickness);
        break;
    }
    case 7:
    {
        // drawPoly
        // for (int n = 0; n < 10; n++)
        bool isReal = false;

        // draw->drawPoly(verticess, sizeof(verticess) / sizeof(verticess[0]), true,
        //                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f), thickness);

        Vec2 vertices[5] = { Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
                            Vec2(50.0f, 100.0f) };
        draw->drawPoly(vertices, 5, false, Color4B::BLUE, thickness);

        Vec2 vertices2[3] = { Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f) };
        draw->drawPoly(vertices2, 3, true, Color4B::GREEN, thickness);

        draw->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4B::RED, thickness);

        break;
    }
    case 8:
    {
        // drawPolygon

        draw->setDNScale(Vec2(thickness, thickness));
        draw->setDNPosition(Vec2(0, 0));
        draw->setDNRotation(0);
        draw->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
            Color4F::YELLOW);
        draw->setDNPosition(Vec2(0, 0));
        draw->setDNRotation(thickness);
        draw->setDNScale(Vec2(thickness, thickness));
        draw->setDNCenter(vertices1[0]);
        draw->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::GREEN, thickness,
            Color4F::YELLOW);

        draw1->clear();
        draw1->setPosition(Vec2(200, 0));
        draw1->setScale(thickness);
        draw1->drawPolygon(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), Color4F::RED, thickness,
            Color4F::YELLOW);

        break;
    }
    case 9:
    {
        // Draw 10 circles/dots
        for (int i = 0; i < count / 100; i++)
        {
            draw->drawDot(Vec2(100.f + AXRANDOM_0_1() * VisibleRect::rightTop().x,
                100.f + AXRANDOM_0_1() * VisibleRect::rightTop().y),
                AXRANDOM_0_1() * 20 + 20, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 10:
    {
        // drawPoint
        for (int i = 0; i < 100; i++)
        {
            draw->drawPoint(Vec2(i * 7.0f, 50.0f), (float)i / 5 + 1,
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 11:
    {
        // drawPoints
        for (int i = 0; i < count; i++)
        {
            Vec2 position[] = {
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y} };
            draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));
        }

        break;
    }
    case 12:
    {
        // drawTriangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawTriangle(pos + Vec2(10.0f, 10.0f), pos + Vec2(70.0f, 30.0f), pos + Vec2(100.0f, 140.0f),
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 13:
    {
        // Draw segment

        int yy1 = 0;
        int yy = 50;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::GREEN, DrawNodeEx::Square, DrawNodeEx::Round);  //default  DrawNodeEx::Round

   //     label1->setPosition(Vec2(410.0f, yy + 55));


        yy += 110;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::BLUE, DrawNodeEx::Butt, DrawNodeEx::Square);
   //     label2->setPosition(Vec2(410.0f, yy));


        yy += 110;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::RED, DrawNodeEx::Round, DrawNodeEx::Butt);
     //   label3->setPosition(Vec2(410.0f, yy - 55));

        break;
    }
    case 14:
    {
        // draw a solid circle
        for (int i = 0; i < count; i++)
        {
            draw->drawSolidCircle(
                Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y),
                AXRANDOM_0_1() * 40.f + 40.f, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 15:
    {
        // draw a solid poly
        draw->resetDNValues();
        draw->setDNPosition(vertices1[0]);
        draw->setDNRotation(count);
        //    draw->setDNScale(Vec2(count, count));
        draw->setDNCenter(vertices1[0]);
        draw->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4F::GREEN, count);
        draw->resetDNValues();
        break;
    }
    case 16:
    {
        // draw a solid rectangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawSolidRect(pos, pos + Vec2(20.0f, 20.0f),
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 17:
    {
        // draw->setPosition(0.1);
        Vec2 gear1 = { 280.f, 320.f };
        Vec2 gear2 = { 160.f, 320.f };
        Vec2 gear3 = { 200.f, 200.f };

        Vec2 gear4 = { s.width - 200, s.height - 200 };
        draw->drawStar(Vec2(gear1), 30, 60, 8, Color4F::BLUE, 4.0);
        draw->setDNRotation(-rotation);
        draw->setDNCenter(gear2);
        draw->drawStar(gear2, 30, 60, 8, Color4F::GREEN, 4.0);

        draw->resetDNValues();
        draw->drawLine(gear2, gear1, Color4F::RED, thickness);  // line
        draw->setDNCenter(gear4);
        draw->setDNRotation(rotation + 45);
        draw->drawStar(gear3, 30, 60, 18, Color4F::RED, 1.0);
        draw->drawLine(gear3, gear4, Color4F::YELLOW, thickness);  // line
        draw->resetDNValues();
        draw->setDNRotation(rotation - 45);
        draw->setDNCenter(gear4);
        draw->drawStar(gear4, 40, 60, 60, Color4F::GREEN, 1.0);




        // draw a star
        //for (int i = 0; i < count; i++)
        //{
        //    Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
        //    draw->drawSolidRect(pos, pos + Vec2(20.0f, 20.0f),
        //        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        //}
        break;
    }
    case 18:
    {

        // draw->setPosition(0.1);
        Vec2 gear1 = { 280.f, 320.f };
        Vec2 gear2 = { 160.f, 320.f };
        Vec2 gear3 = { 200.f, 200.f };

        Vec2 gear4 = { s.width - 200, s.height - 200 };
        draw->drawSolidStar(Vec2(gear1), 30, 60, 8, Color4F::BLUE, Color4F::BLUE, 4.0);
        draw->setDNRotation(-rotation);
        draw->setDNCenter(gear2);
        draw->drawSolidStar(gear2, 30, 60, 8, Color4F::GREEN, Color4F::YELLOW, 4.0);

        draw->resetDNValues();
        draw->drawLine(gear2, gear1, Color4F::RED, thickness);  // line
        draw->setDNCenter(gear4);
        draw->setDNRotation(rotation + 45);
        draw->drawSolidStar(gear3, 30, 60, 18, Color4F::RED, Color4F::BLUE, 1.0);
        draw->drawLine(gear3, gear4, Color4F::YELLOW, thickness);  // line
        draw->resetDNValues();
        draw->setDNRotation(rotation - 45);
        draw->setDNCenter(gear4);
        draw->drawSolidStar(gear4, 40, 60, 60, Color4F::GREEN, Color4F::BLUE, 1.0);
        // draw a solid star
        //for (int i = 0; i < count; i++)
        //{
        //    Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
        //    draw->drawSolidRect(pos, pos + Vec2(20.0f, 20.0f),
        //        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        //}
        break;
    }

    default:
        break;
    }
}

// DrawFilledPrimitivesTest
DrawNodePerformaneTest::DrawNodePerformaneTest()
{
    _currentSeletedItemIndex = 0;

    auto director = Director::getInstance();
    director->setClearColor(Color4F(0, 0, 0, 0));
    auto origin = director->getVisibleOrigin();
    auto size = director->getVisibleSize();
    Vec2 center(origin.x + size.width / 2 + 50, origin.y + size.height / 2);

    auto listview = createListView();
    listview->setPosition(Vec2(0.0f, 90.0f));
    addChild(listview);

    draw = DrawNodeEx::create();
    draw->setScale(0.5);
    draw->setPosition(size / 4);
    addChild(draw);

    auto thicknessSlider = createSlider();
    thicknessSlider->setPosition(Vec2(center.x, 60.0f));
    addChild(thicknessSlider);

    label = Label::createWithTTF("Count: ", "fonts/Marker Felt.ttf", 10);
    label->setPosition(Vec2(Vec2(center.x, 80.0f)));
    this->addChild(label, 1);

    label1 = Label::createWithTTF("DrawNodeEx::Round", "fonts/Arial.ttf", 12);
    addChild(label1, 1);
    label2 = Label::createWithTTF("DrawNodeEx::Square", "fonts/Arial.ttf", 12);
    addChild(label2, 1);
    label3 = Label::createWithTTF("DrawNodeEx::Butt", "fonts/Arial.ttf", 12);
    addChild(label3, 1);

    scheduleUpdate();
}

std::string DrawNodePerformaneTest::title() const
{
    return "DrawNode #2 Test";
}

string DrawNodePerformaneTest::subtitle() const
{
    return "";
}

ax::ui::Slider* DrawNodePerformaneTest::createSlider()
{
    auto slider = ui::Slider::create();
    slider->setTouchEnabled(true);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->addEventListener(AX_CALLBACK_2(DrawNodePerformaneTest::sliderCallback, this));
    //     slider->setRotation(90);
    slider->setTag(101);
    slider->setPercent(10);

    return slider;
}

void DrawNodePerformaneTest::listviewCallback(ax::Object* sender, ax::ui::ListView::EventType type)
{
    // clear all text to white
    auto listview = static_cast<ax::ui::ListView*>(sender);
    for (auto&& item : listview->getItems())
    {
        static_cast<ax::ui::Text*>(item)->setColor(ax::Color3B::WHITE);
    }
    _currentSeletedItemIndex = (int)listview->getCurSelectedIndex();
    listview->getItem(_currentSeletedItemIndex)->setColor(ax::Color3B::RED);
}

void DrawNodePerformaneTest::sliderCallback(ax::Object* sender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = dynamic_cast<ax::ui::Slider*>(sender);
        count = slider->getPercent() * 10;
    }
}

ax::ui::ListView* DrawNodePerformaneTest::createListView()
{
    auto listview = ax::ui::ListView::create();

    auto drawLine = ax::ui::Text::create();
    drawLine->setString("drawDot");
    drawLine->setTouchEnabled(true);
    listview->pushBackCustomItem(drawLine);

    auto drawSegment = ax::ui::Text::create();
    drawSegment->setString("drawPoint");
    drawSegment->setTouchEnabled(true);
    listview->pushBackCustomItem(drawSegment);

    auto drawTriangle = ax::ui::Text::create();
    drawTriangle->setString("drawPoints");
    drawTriangle->setTouchEnabled(true);
    listview->pushBackCustomItem(drawTriangle);

    auto drawRect = ax::ui::Text::create();
    drawRect->setString("drawTriangle");
    drawRect->setTouchEnabled(true);
    listview->pushBackCustomItem(drawRect);

    auto drawCircle = ax::ui::Text::create();
    drawCircle->setString("drawSegment");
    drawCircle->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCircle);

    auto drawQuadBezier = ax::ui::Text::create();
    drawQuadBezier->setString("drawSolidCircle");
    drawQuadBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawQuadBezier);

    auto drawCubicBezier = ax::ui::Text::create();
    drawCubicBezier->setString("drawSolidPoly");
    drawCubicBezier->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCubicBezier);

    auto drawCardinalSpline = ax::ui::Text::create();
    drawCardinalSpline->setString("drawSolidRect");
    drawCardinalSpline->setTouchEnabled(true);
    listview->pushBackCustomItem(drawCardinalSpline);

    listview->setContentSize(drawLine->getContentSize() * 8);
    listview->setCurSelectedIndex(0);
    listview->setTouchEnabled(true);
    listview->addEventListener(
        (ui::ListView::ccListViewCallback)AX_CALLBACK_2(DrawNodePerformaneTest::listviewCallback, this));
    listview->setTag(100);

    listview->getItem(_currentSeletedItemIndex)->setColor(Color3B::RED);

    return listview;
}

void DrawNodePerformaneTest::update(float dt)
{

    auto s = Director::getInstance()->getWinSize();

    draw->clear();

    label1->setVisible(false);
    label2->setVisible(false);
    label3->setVisible(false);




    label->setString("Count: (" + Value(count).asString() + ")");
    switch (_currentSeletedItemIndex)
    {
    case 0:
    {
        // Draw 10 circles/dots
        for (int i = 0; i < count / 100; i++)
        {
            draw->drawDot(Vec2(100.f + AXRANDOM_0_1() * VisibleRect::rightTop().x,
                100.f + AXRANDOM_0_1() * VisibleRect::rightTop().y),
                AXRANDOM_0_1() * 20 + 20, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 1:
    {
        for (int i = 0; i < 100; i++)
        {
            draw->drawPoint(Vec2(i * 7.0f, 50.0f), (float)i / 5 + 1,
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
        }
        break;
    }
    case 2:
    {
        // drawPoints
        for (int i = 0; i < count; i++)
        {
            Vec2 position[] = {
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
                {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y} };
            draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));
        }

        break;
    }
    case 3:
    {
        // drawTriangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawTriangle(pos + Vec2(10.0f, 10.0f), pos + Vec2(70.0f, 30.0f), pos + Vec2(100.0f, 140.0f),
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 4:
    {
        // Draw segment
        label1->setVisible(true);
        label2->setVisible(true);
        label3->setVisible(true);

        int yy1 = 0;
        int yy = 50;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::GREEN, DrawNodeEx::Round, DrawNodeEx::Round); 

        label1->setPosition(Vec2(410.0f, yy + 55));


        yy += 110;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::BLUE, DrawNodeEx::Round, DrawNodeEx::Square);
        label2->setPosition(Vec2(410.0f, yy));


        yy += 110;
        draw->drawSegment(Vec2(50.0f, yy), Vec2(400, yy - yy1), count / 20,
            Color4F::RED, DrawNodeEx::Round, DrawNodeEx::Butt);
        label3->setPosition(Vec2(410.0f, yy - 55));

        break;
    }
    case 5:
    {
        // draw a solid circle
        for (int i = 0; i < count; i++)
        {
            draw->drawSolidCircle(
                Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y),
                AXRANDOM_0_1() * 40.f + 40.f, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    case 6:
    {
        // draw a solid poly
        draw->resetDNValues();
        draw->setDNPosition(vertices1[0]);
        draw->setDNRotation(count);
        //    draw->setDNScale(Vec2(count, count));
        draw->setDNCenter(vertices1[0]);
        draw->drawPoly(vertices1, sizeof(vertices1) / sizeof(vertices1[0]), true, Color4F::GREEN, count);
        draw->resetDNValues();
        break;
    }
    case 7:
    {
        // draw a solid rectangle
        for (int i = 0; i < count; i++)
        {
            Vec2 pos = Vec2(AXRANDOM_0_1() * VisibleRect::rightTop().x, AXRANDOM_0_1() * VisibleRect::rightTop().y);
            draw->drawSolidRect(pos, pos + Vec2(20.0f, 20.0f),
                Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));
        }
        break;
    }
    default:
        break;
    }
}

DrawNodeHeartTest::DrawNodeHeartTest()
{
    auto s = Director::getInstance()->getWinSize();
    //// https://virusinlinux.github.io/heartAnimation/

    heart = new Vec2[totalFrames];

    for (int i = 0; i < totalFrames; i++)
    {
        float a = AXRANDOM_0_1() * M_PI * 4;
        float r = AXRANDOM_0_1() * sin(a);
        heart[i] = { r * 100, r * 100 };
        heart[i] = { s.width / 2,s.height / 2 };
    }
    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);
    scheduleUpdate();
}

std::string DrawNodeHeartTest::title() const
{
    return "Heart Animation";
}
std::string DrawNodeHeartTest::subtitle() const
{
    return "";
}
void DrawNodeHeartTest::update(float dt)
{
    auto s = Director::getInstance()->getWinSize();
    static int counter = 0;
    //function draw() {
    //
    //
    float percent = float(counter % totalFrames) / totalFrames;
    //    render(percent);

    //}
    //
    //function render(percent) {
    //    background(0);
    //    translate(width / 2, height / 2);
    //    stroke(255, 0, 100);
    //    strokeWeight(4);
    //    fill(255, 0, 100);
    //
    //    beginShape();
    //   
    //        
    //       
    //        
    //
    //    }
    for (int i = 0; i < totalFrames; i++) // for (let v of heart) {
    {
        float a = percent * M_PI * 2;           // const a = map(percent, 0, 1, 0, TWO_PI * 2);
        float r = AXRANDOM_0_1() * sin(a);      //  const r = map(sin(a), -1, 1, height / 80, height / 40);
        //     Vec2 vertex = { r * v.x, r * v.y } ;    //  vertex(r * v.x, r * v.y);          //  heart[i] = { r * heart[i].x, r * heart[i].y };
        // heart.splice(0, 1);             //   The splice() method of Array instances changes the contents of an array by removing or replacing existing elements and/or adding new elements in place.
        if (percent < 0.5)  //  if (percent < 0.5) {
        {
            float a = percent * M_PI * 2;       // const a = map(percent, 0, 0.5, 0, TWO_PI);
            float r = AXRANDOM_0_1() * sin(a);  // const x = 16 * pow(sin(a), 3);
            float y = 1;                        // const y = -(13 * cos(a) - 5 * cos(2 * a) - 2 * cos(3 * a) - cos(4 * a));
            // heart.push(createVector(x, y));
        }
        heart[i] = { r * heart[i].x, r * heart[i].y };
    }



    drawNodeEx->clear();
    drawNodeEx->setIsConvex(true);
    drawNodeEx->drawPolygon(heart, totalFrames, 1.0, Color4B::RED);
    drawNodeEx->setIsConvex(false);
    //    endShape();
    //


    drawNodeEx->drawStar({ s.width / 2, s.height / 2 }, 40, 20, 8, Color4B::BLUE);
    counter++;
}

DrawNodeDrawInWrongOrder_Issue1888::DrawNodeDrawInWrongOrder_Issue1888()
{
    auto s = Director::getInstance()->getWinSize();
    //// https://virusinlinux.github.io/heartAnimation/

    heart = new Vec2[totalFrames];

    for (int i = 0; i < totalFrames; i++)
    {
        float a = AXRANDOM_0_1() * M_PI * 4;
        float r = AXRANDOM_0_1() * sin(a);
        heart[i] = { r * 100, r * 100 };
        heart[i] = { s.width / 2,s.height / 2 };
    }
    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);
    scheduleUpdate();
}

std::string DrawNodeDrawInWrongOrder_Issue1888::title() const
{
    return "Issue #1888: Drawing order";
}

std::string DrawNodeDrawInWrongOrder_Issue1888::subtitle() const
{
    return "Red behind all. Green behind the blue.\nRandom Points behind the square. Blue is top.";
}

void DrawNodeDrawInWrongOrder_Issue1888::update(float dt)
{
    drawNodeEx->clear();

#if defined(DRAWNODE_DRAW_LINE_POINT)
    drawNodeEx->_drawOrder = true;
#endif


    drawNodeEx->drawLine(Vec2(20, 140), Vec2(450, 110), Color4B::RED, 20.0f);

    Vec2 position1[] = {
        {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
        {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
        {60 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 60 + AXRANDOM_0_1() * VisibleRect::rightTop().y},
        {70 + AXRANDOM_0_1() * VisibleRect::rightTop().x, 70 + AXRANDOM_0_1() * VisibleRect::rightTop().y} };
    drawNodeEx->drawPoints(position1, 4, 10, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    drawNodeEx->drawSolidRect(Vec2(250, 80), Vec2(400, 220), Color4B::YELLOW);
    for (int i = 0; i < 100; i++)
    {
        drawNodeEx->drawPoint(Vec2(i * 7.0f, 120.0f), (float)i / 5 + 1,
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }
    drawNodeEx->drawLine(Vec2(20, 100), Vec2(450, 220), Color4B::GREEN, 8.0f);
    drawNodeEx->drawLine(Vec2(200, 100), Vec2(450, 250), Color4B::BLUE,6.0f);


#if defined(DRAWNODE_DRAW_LINE_POINT)
    drawNodeEx->_drawOrder = true;
#endif
}


// DrawNodeAxmolTest2
DrawNodeAxmolTest2::DrawNodeAxmolTest2()
{
    auto s = Director::getInstance()->getWinSize();

    auto draw = DrawNodeEx::create();
    addChild(draw, 10);

    draw->drawPoint(Vec2(s.width / 2 - 120, s.height / 2 - 120), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    draw->drawPoint(Vec2(s.width / 2 + 120, s.height / 2 + 120), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw 4 small points
    Vec2 position[] = { Vec2(60, 60), Vec2(70, 70), Vec2(60, 70), Vec2(70, 60) };
    draw->drawPoints(position, 4, 5, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a line
    draw->drawLine(Vec2(0, 0), Vec2(s.width, s.height), Color4F(1.0, 0.0, 0.0, 0.5));

    // draw a rectangle
    draw->drawRect(Vec2(23, 23), Vec2(7, 7), Color4F(1, 1, 0, 1));

    draw->drawRect(Vec2(15, 30), Vec2(30, 15), Vec2(15, 0), Vec2(0, 15),
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1));

    // draw a circle
    draw->drawCircle(VisibleRect::center() + Vec2(140, 0), 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f, 2.0f,
        Color4F(1.0f, 0.0f, 0.0f, 0.5f));

    draw->drawCircle(VisibleRect::center() - Vec2(140, 0), 50, AX_DEGREES_TO_RADIANS(90), 30, false,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw some beziers
    draw->drawQuadBezier(Vec2(s.width - 150, s.height - 150), Vec2(s.width - 70, s.height - 10),
        Vec2(s.width - 10, s.height - 10), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawQuadBezier(Vec2(0.0f, s.height), Vec2(s.width / 2, s.height / 2), Vec2(s.width, s.height), 50,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(VisibleRect::center(), Vec2(VisibleRect::center().x + 30, VisibleRect::center().y + 50),
        Vec2(VisibleRect::center().x + 60, VisibleRect::center().y - 50), VisibleRect::right(), 100,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    draw->drawCubicBezier(Vec2(s.width - 250, 40.0f), Vec2(s.width - 70, 100.0f), Vec2(s.width - 30, 250.0f),
        Vec2(s.width - 10, s.height - 50), 10,
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array = PointArray::create(20);
    array->addControlPoint(Vec2(0.0f, 0.0f));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, 80.0f));
    array->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array->addControlPoint(Vec2(80.0f, s.height - 80));
    array->addControlPoint(Vec2(80.0f, 80.0f));
    array->addControlPoint(Vec2(s.width / 2, s.height / 2));
    draw->drawCardinalSpline(array, 0.5f, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    auto array2 = PointArray::create(20);
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, 30.0f));
    array2->addControlPoint(Vec2(s.width - 80, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, s.height - 80));
    array2->addControlPoint(Vec2(s.width / 2, 30.0f));
    draw->drawCatmullRom(array2, 50, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    // open random color poly
    Vec2 vertices[] = { Vec2(0.0f, 0.0f), Vec2(50.0f, 50.0f), Vec2(100.0f, 50.0f), Vec2(100.0f, 100.0f),
        Vec2(50.0f, 100.0f) };
    draw->drawPoly(vertices, 5, false, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // closed random color poly
    Vec2 vertices2[] = { Vec2(30.0f, 130.0f), Vec2(30.0f, 230.0f), Vec2(50.0f, 200.0f) };
    draw->drawPoly(vertices2, 3, true, Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));

    // Draw 10 circles
    for (int i = 0; i < 10; i++)
    {
        draw->drawDot(Vec2(s.width / 2, s.height / 2), 10.f * (10 - i),
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    // Draw polygons
    Vec2 points[] = { Vec2(s.height / 4, 0.0f), Vec2(s.width, s.height / 5), Vec2(s.width / 3 * 2, s.height) };
    draw->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 4,
        Color4F(0.0f, 0.0f, 1.0f, 0.5f));

    // star poly (triggers buggs)
    {
        const float o = 80;
        const float w = 20;
        const float h = 50;
        Vec2 star[] = {
            Vec2(o + w, o - h), Vec2(o + w * 2, o),                  // lower spike
            Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2),  // right spike
            //              {o +w, o+w*2+h}, {o,o+w*2},                 // top spike
            //              {o -h, o+w}, {o,o},                         // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
            Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // star poly (doesn't trigger bug... order is important un tesselation is supported.
    {
        const float o = 180;
        const float w = 20;
        const float h = 50;
        Vec2 star[] = {
            Vec2(o, o),
            Vec2(o + w, o - h),
            Vec2(o + w * 2, o),  // lower spike
            Vec2(o + w * 2 + h, o + w),
            Vec2(o + w * 2, o + w * 2),  // right spike
            Vec2(o + w, o + w * 2 + h),
            Vec2(o, o + w * 2),  // top spike
            Vec2(o - h, o + w),  // left spike
        };

        draw->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1,
            Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // draw a solid polygon
    Vec2 vertices3[] = { Vec2(60.0f, 160.0f), Vec2(70.0f, 190.0f), Vec2(100.0f, 190.0f), Vec2(90.0f, 160.0f) };
    draw->drawSolidPoly(vertices3, 4, Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid rectangle
    draw->drawSolidRect(Vec2(10.0f, 10.0f), Vec2(20.0f, 20.0f), Color4F(1.0f, 1.0f, 0.0f, 1.0f));

    // draw a solid circle
    draw->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 50, 2.0f, 2.0f,
        Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    // Draw segment
    draw->drawSegment(Vec2(20.0f, s.height), Vec2(20.0f, s.height / 2), 10, Color4F(0.0f, 1.0f, 0.0f, 1.0f));

    draw->drawSegment(Vec2(10.0f, s.height / 2), Vec2(s.width / 2, s.height / 2), 40, Color4F(1.0f, 0.0f, 1.0f, 0.5f));

    // Draw triangle
    draw->drawTriangle(Vec2(10.0f, 10.0f), Vec2(70.0f, 30.0f), Vec2(100.0f, 140.0f),
        Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 0.5f));

    for (int i = 0; i < 100; i++)
    {
        draw->drawPoint(Vec2(i * 7.0f, 5.0f), (float)i / 5 + 1,
            Color4F(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), 1.0f));
    }

    auto draw1 = DrawNodeEx::create();
    this->addChild(draw1, 10);
    draw1->setLineWidth(4);
    draw1->drawLine(Vec2(0.0f, s.height), Vec2(s.width, s.height - 20), Color4F::YELLOW);
    draw1->drawLine(Vec2(0.0f, 0.0f), Vec2(s.width, s.height - 20), Color4F::YELLOW);

    draw->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2f), FadeOut::create(1.2f), NULL)));
    draw1->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1.2f), FadeOut::create(1.2f), NULL)));
}

string DrawNodeAxmolTest2::title() const
{
    return "Axmol DrawNode v2 test";
}

string DrawNodeAxmolTest2::subtitle() const
{
    return "";
}

//
// Issue11942Test
//
DrawNodeCocos2dxBackwardsAPITest::DrawNodeCocos2dxBackwardsAPITest()
{
    float o = 80;
    float w = 20;
    float h = 50;

    auto drawNode1 = DrawNodeEx::create();
    addChild(drawNode1);
    drawNode1->setPosition(-40, 20);

    int x = 0;
    int y = 0;
    Vec2 vertices[4];
    drawNode1->setScale(0.5);
    Color4F color;
    for (int iy = 0; iy < 5; iy++)
    {
        x = 0;
        for (int ix = 0; ix < 13; ix++)
        {
            vertices[0] = Vec2(x + o + w, y + o - h);
            vertices[1] = Vec2(x + o + w * 2, y + o);
            vertices[2] = Vec2(x + o + w * 2 + h, y + o + w);
            vertices[3] = Vec2(x + o + w * 2, y + o + w * 2);

            if (AXRANDOM_0_1() > 0.5f)
            {
                drawNode1->setIsConvex(true);
                color = Color4F::YELLOW;
            }
            else
            {
                drawNode1->setIsConvex(false);  // default value!
                color = Color4F::ORANGE;
            }
            drawNode1->drawPolygon(vertices, 4, Color4F(0.7f, 0.7f, 0.7f, 0.5f), 1, color);
            x += 70;
        }
        y += 80;
    }
}

string DrawNodeCocos2dxBackwardsAPITest::title() const
{
    return "API backwards compatible test";
}

string DrawNodeCocos2dxBackwardsAPITest::subtitle() const
{
    return "YELLOW=cocos2dx/axmol <=2.0 ORANGE=axmol >2.0";
}

//
// drawCircle new feature (better rendering)
//
DrawNodeCocos2dxBetterCircleRendering::DrawNodeCocos2dxBetterCircleRendering()
{
    // Add lines to see the correct "scale of the 'rings'" changing the window size

    auto draw = DrawNodeEx::create();
    draw->setLineWidth(1);
    addChild(draw, 10);

    for (float y = 0; y < VisibleRect::top().y; y += 10)
    {
        draw->drawLine({ VisibleRect::left().x, y }, { VisibleRect::right().x, y }, Color4B::GRAY);
    }
    initSliders();

    drawNode = DrawNodeEx::create();
    addChild(drawNode, 10);

    lineWidth = 0;

    scheduleUpdate();
}

void DrawNodeCocos2dxBetterCircleRendering::changeLineWidth(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderLineWidth = dynamic_cast<ax::ui::Slider*>(pSender);
        lineWidth = static_cast<float>(sliderLineWidth->getPercent());
        _lineWidthLabel->setString("setLineWidth(" + Value(lineWidth).asString() + ")");
    }
}

void DrawNodeCocos2dxBetterCircleRendering::changeThreshold(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sliderThreshold = dynamic_cast<ax::ui::Slider*>(pSender);
        threshold = static_cast<float>(sliderThreshold->getPercent());
        _thresholdLabel->setString("drawCircle(pos, radius, ..., segments, ..., color, " + Value(threshold).asString() +
            ")");
    }
}

void DrawNodeCocos2dxBetterCircleRendering::initSliders()
{
    auto vsize = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* slider = ax::ui::Slider::create();
    slider->setPercent(0);
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->setPosition(Vec2(vsize.width / 2, vsize.height / 6));

    slider->addEventListener(AX_CALLBACK_2(DrawNodeCocos2dxBetterCircleRendering::changeThreshold, this));

    auto ttfConfig = TTFConfig("fonts/arial.ttf", 8);
    _thresholdLabel = Label::createWithTTF(ttfConfig, "drawCircle(pos, radius, ..., segments, ..., color, 0)");
    addChild(_thresholdLabel, 20);
    _thresholdLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 15));

    addChild(slider, 20);

    ax::ui::Slider* sliderLineWidth = ax::ui::Slider::create();
    sliderLineWidth->setPercent(0);
    sliderLineWidth->loadBarTexture("cocosui/sliderTrack.png");
    sliderLineWidth->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderLineWidth->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderLineWidth->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 35));
    sliderLineWidth->addEventListener(AX_CALLBACK_2(DrawNodeCocos2dxBetterCircleRendering::changeLineWidth, this));

    _lineWidthLabel = Label::createWithTTF(ttfConfig, "setLineWidth(0)");

    addChild(_lineWidthLabel, 20);
    _lineWidthLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 50));
    addChild(sliderLineWidth, 20);
}

void DrawNodeCocos2dxBetterCircleRendering::update(float dt)
{
    drawNode->clear();
    drawNode->setLineWidth(lineWidth);  // value from the slider

    // Old behavior => faster but badly rendering if line width > 5 (= rings)
    drawNode->drawCircle(VisibleRect::center() + Vec2(120.0f, 0.0f), 60, AX_DEGREES_TO_RADIANS(90), 36, false,
        Color4F::RED);

    // New behavior => slower but good rendering if line width > 5
    auto color = Color4F::GREEN;
    if (lineWidth <= threshold)
    {
        color = Color4F::RED;  // using the faster rendering internal method of drawCircle (old behavior)
    }
    drawNode->drawCircle(VisibleRect::center() - Vec2(120.0f, 0.0f), 60, AX_DEGREES_TO_RADIANS(90), 36, false, color,
        threshold);
}

string DrawNodeCocos2dxBetterCircleRendering::title() const
{
    return "Rendering drawCircle";
}

string DrawNodeCocos2dxBetterCircleRendering::subtitle() const
{
    return "Green: smoother rendering; Red: faster but badly rendering";
}

DrawNodeCocos2dxDrawNodePieTest::DrawNodeCocos2dxDrawNodePieTest()
{
    drawNode = DrawNodeEx::create();
    addChild(drawNode, 10);

    initSliders();

    scheduleUpdate();
}

void DrawNodeCocos2dxDrawNodePieTest::changeEndAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sEndAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        endAngle = sEndAngle->getPercent() * 3.6;
        _EndAngleLabel->setString("endAngle: (" + Value(endAngle).asString() + ")");
    }
}

void DrawNodeCocos2dxDrawNodePieTest::changeStartAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        startAngle = sStartAngle->getPercent() * 3.6;
        _StartAngleLabel->setString("startAngle: (" + Value(startAngle).asString() + ")");
    }
}

void DrawNodeCocos2dxDrawNodePieTest::changeAngle(ax::Object* pSender, ax::ui::Slider::EventType type)
{
    if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        ax::ui::Slider* sStartAngle = dynamic_cast<ax::ui::Slider*>(pSender);
        angle = sStartAngle->getPercent() * 3.6;
        _AngleLabel->setString("Angle: (" + Value(angle).asString() + ")");
    }
}

void DrawNodeCocos2dxDrawNodePieTest::initSliders()
{
    angle = 324;
    endAngle = 360;
    startAngle = 0;

    auto vsize = Director::getInstance()->getVisibleSize();
    ax::ui::Slider* sliderStartAngle = ax::ui::Slider::create();
    sliderStartAngle->setPercent(startAngle);
    sliderStartAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderStartAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderStartAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderStartAngle->setPosition(Vec2(vsize.width / 2, vsize.height / 6));

    sliderStartAngle->addEventListener(AX_CALLBACK_2(DrawNodeCocos2dxDrawNodePieTest::changeStartAngle, this));

    auto ttfConfig = TTFConfig("fonts/arial.ttf", 8);
    _StartAngleLabel = Label::createWithTTF(ttfConfig, "StartAngle");
    addChild(_StartAngleLabel, 20);
    _StartAngleLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 15));

    addChild(sliderStartAngle, 20);

    ax::ui::Slider* sliderEndAngle = ax::ui::Slider::create();
    sliderEndAngle->setPercent(endAngle);
    sliderEndAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderEndAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderEndAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderEndAngle->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 35));
    sliderEndAngle->addEventListener(AX_CALLBACK_2(DrawNodeCocos2dxDrawNodePieTest::changeEndAngle, this));

    _EndAngleLabel = Label::createWithTTF(ttfConfig, "endAngle");

    addChild(_EndAngleLabel, 20);
    _EndAngleLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 50));
    addChild(sliderEndAngle, 20);

    ax::ui::Slider* sliderAngle = ax::ui::Slider::create();
    sliderAngle->setPercent(angle);
    sliderAngle->loadBarTexture("cocosui/sliderTrack.png");
    sliderAngle->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    sliderAngle->loadProgressBarTexture("cocosui/sliderProgress.png");
    sliderAngle->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 65));
    sliderAngle->addEventListener(AX_CALLBACK_2(DrawNodeCocos2dxDrawNodePieTest::changeAngle, this));

    _AngleLabel = Label::createWithTTF(ttfConfig, "Angle");

    addChild(_AngleLabel, 20);
    _AngleLabel->setPosition(Vec2(vsize.width / 2, vsize.height / 6 + 80));
    addChild(sliderAngle, 20);
}

void DrawNodeCocos2dxDrawNodePieTest::update(float dt)
{
    drawNode->clear();

    // Pie
    drawNode->drawPie(VisibleRect::center() - Vec2(170.0f, -35.0f), 50, angle, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::BLUE, Color4F::BLUE, drawNode->DrawMode::Fill);

    drawNode->drawPie(VisibleRect::center() - Vec2(60.0f, -35.0f), 50, angle, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::BLUE, Color4F::BLUE, drawNode->DrawMode::Outline, 1.0f);
    drawNode->drawPie(VisibleRect::center() + Vec2(60.0f, 35.0f), 50, angle, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::BLUE, Color4F::BLUE, drawNode->DrawMode::Line, 1.0f);
    drawNode->drawPie(VisibleRect::center() + Vec2(170.0f, 35.0f), 50, angle, startAngle, endAngle, 1.0f, 1.0f,
        Color4F::BLUE, Color4F::BLUE, drawNode->DrawMode::Semi, 1.0f);
}

string DrawNodeCocos2dxDrawNodePieTest::title() const
{
    return "DrawNode::drawPie";
}

string DrawNodeCocos2dxDrawNodePieTest::subtitle() const
{
    return "Filled, Outlined, Line, Semi";
}

DrawNodeCocos2dx_Issue829::DrawNodeCocos2dx_Issue829()
{
    Vec2 vertices0[] = { {50.0, 20.0}, {100.0, 0.0}, {80.0, 50.0}, {100.0, 100.0},
        {50.0, 80.0}, {0.0, 100.0}, {20.0, 50.0}, {0, 0} };

    Vec2 vertices4[] = { {362, 148}, {326, 241}, {295, 219}, {258, 88}, {440, 129},
        {370, 196}, {372, 275}, {348, 257}, {364, 148} };

    Vec2* ver[] = { vertices0, vertices4 };

    DrawNodeEx* drawNode[sizeof(ver) + 1];
    for (int i = 0; i < sizeof(ver); i++)
    {
        drawNode[i] = DrawNodeEx::create();
        addChild(drawNode[i]);
    }

    drawNode[0]->drawPolygon(vertices0, sizeof(vertices0) / sizeof(vertices0[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 3,
        Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    drawNode[0]->setPosition({ 20, 200 });
    drawDirection(vertices0, sizeof(vertices0) / sizeof(vertices0[0]), drawNode[0]->getPosition());

    drawNode[4]->drawPolygon(vertices4, sizeof(vertices4) / sizeof(vertices4[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 3,
        Color4F(0.0f, 0.0f, 1.0f, 1.0f));
    drawNode[4]->setPosition({ -70, -20 });
    drawDirection(vertices4, sizeof(vertices4) / sizeof(vertices4[0]), drawNode[4]->getPosition());

    {
        const float o = 80;
        const float w = 20;
        const float h = 50;
        {  // star
            auto drawNode1 = DrawNodeEx::create();
            addChild(drawNode1);
            drawNode1->setPosition(300, 100);
            Vec2 star[] = {
                Vec2(o, o),
                Vec2(o + w, o - h),
                Vec2(o + w * 2, o),  // lower spike
                Vec2(o + w * 2 + h, o + w),
                Vec2(o + w * 2, o + w * 2),  // right spike
                Vec2(o + w, o + w * 2 + h),
                Vec2(o, o + w * 2),  // top spike
                Vec2(o - h, o + w),  // left spike
            };

            drawNode1->drawPolygon(star, sizeof(star) / sizeof(star[0]), Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1,
                Color4F(0.0f, 0.0f, 1.0f, 1.0f));

            drawDirection(star, sizeof(star) / sizeof(star[0]), drawNode1->getPosition());
        }

        {  // wrong order
            auto drawNode1 = DrawNodeEx::create();
            addChild(drawNode1);
            drawNode1->setPosition(-80, 20);
            Vec2 wrongOrder[] = { Vec2(o + w, o - h), Vec2(o + w * 2, o), Vec2(o + w * 2 + h, o + w),
                Vec2(o + w * 2, o + w * 2) };

            drawNode1->drawPolygon(wrongOrder, sizeof(wrongOrder) / sizeof(wrongOrder[0]),
                Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1, Color4F(1.0f, 0.0f, 0.0f, 1.0f));

            drawDirection(wrongOrder, sizeof(wrongOrder) / sizeof(wrongOrder[0]), drawNode1->getPosition());
        }
        {  // correct order
            Vec2 correctOrder[] = { Vec2(o + w * 2, o), Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2),
                Vec2(o + w, o - h) };
            auto drawNode2 = DrawNodeEx::create();
            addChild(drawNode2);
            drawNode2->setPosition({ -10, 20 });
            drawNode2->drawPolygon(correctOrder, sizeof(correctOrder) / sizeof(correctOrder[0]),
                Color4F(0.0f, 0.0f, 0.7f, 0.5f), 1, Color4F(0.0f, 1.0f, 0.0f, 1.0f));

            drawDirection(correctOrder, sizeof(correctOrder) / sizeof(correctOrder[0]), drawNode2->getPosition());
        }
    }
}

void DrawNodeCocos2dx_Issue829::drawDirection(const Vec2* vec, const int size, Vec2 offset)
{
    for (size_t i = 0; i < size; i++)
    {
        auto label = Label::createWithTTF(std::to_string(i).c_str(), "fonts/Marker Felt.ttf", 10);
        this->addChild(label);
        label->setPosition(vec[i] + offset);
    }
}

string DrawNodeCocos2dx_Issue829::title() const
{
    return "GitHub Issue #829";
}

string DrawNodeCocos2dx_Issue829::subtitle() const
{
    return "Is also fixed with #1319!";
}

DrawNodeCocos2dx_Issue1319::DrawNodeCocos2dx_Issue1319()
{
    Vec2 vertices21[] = {
        {290.250000f, 98.1250000f}, {235.000000f, 90.8750000f}, {270.500000f, 109.875000f}, {235.000000f, 119.125000f},
        {275.250000f, 145.875000f}, {249.500000f, 145.875000f}, {249.500000f, 178.125000f}, {275.250000f, 187.375015f},
        {294.750488f, 168.333344f}, {311.250000f, 181.125000f}, {257.000000f, 213.625015f}, {338.500000f, 193.125000f},
        {300.000000f, 211.125015f}, {333.750000f, 211.125015f}, {368.250000f, 206.625000f}, {377.000000f, 178.125000f},
        {421.750000f, 170.125000f}, {416.250000f, 115.375000f}, {391.250000f, 157.875000f}, {338.500000f, 131.625000f},
        {362.750000f, 131.625000f}, {362.750000f, 106.875000f}, {306.500000f, 119.125000f}, {324.250000f, 85.1250000f},
        {227.500000f, 61.8750000} };

    Vec2 vertices24[] = {
        {45.750000f, 144.375000f},  {75.500000f, 136.875000f},  {75.500000f, 159.125000f},  {100.250000f, 161.375000f},
        {65.500000f, 181.375000f},  {102.250000f, 179.125000f}, {95.000000f, 215.125000f},  {129.331467f, 189.926208f},
        {131.371460f, 206.366196f}, {139.651474f, 192.446198f}, {161.851471f, 200.606201f}, {151.000000f, 220.375000f},
        {110.500000f, 244.375000f}, {153.750000f, 238.125000f}, {142.500000f, 253.875000f}, {220.750000f, 259.375000f},
        {250.500000f, 244.375000f}, {168.750000f, 241.875000f}, {182.250000f, 154.125000f}, {190.250000f, 227.375000f},
        {196.500000f, 197.375000f}, {208.750000f, 210.625000f}, {220.750000f, 194.375000f}, {208.750000f, 176.375000f},
        {253.250000f, 173.875000f}, {243.750000f, 154.125000f}, {213.750000f, 161.375000f}, {202.250000f, 139.875000f},
        {236.000000f, 131.875000f}, {218.500000f, 120.875000f}, {206.500000f, 125.625000f}, {184.500000f, 110.375000f},
        {157.000000f, 108.625000f}, {147.500000f, 96.625000f},  {153.750000f, 85.125000f},  {147.500000f, 75.375000f},
        {126.500000f, 74.125000f},  {110.500000f, 86.625000f},  {127.750000f, 85.125000f},  {135.250000f, 91.125000f},
        {135.250000f, 97.875000f},  {124.000000f, 93.875000f},  {115.500000f, 100.875000f}, {115.500000f, 111.875000f},
        {135.250000f, 108.625000f}, {151.000000f, 124.125000f}, {90.500000f, 131.875000f},  {113.250000f, 120.875000f},
        {88.000000f, 116.875000f},  {106.000000f, 103.875000f}, {88.000000f, 97.875000f},
    };

    DrawNodeEx* drawNode[3];
    for (int i = 0; i < 3; i++)
    {
        drawNode[i] = DrawNodeEx::create();
        addChild(drawNode[i]);
    }

    // draw a solid circle
    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(140.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f, 2.0f, Color4F::BLUE);

    drawNode[1]->drawSolidCircle(VisibleRect::center() + Vec2(-40.0f, 0.0f), 40, AX_DEGREES_TO_RADIANS(90), 30, 2.0f, 2.0f, Color4F::WHITE);

    drawNode[0]->setPosition(Vec2(-30, -20));
    drawNode[0]->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4F::RED, 0.3f, Color4F::GREEN);

    drawNode[2]->setPosition(Vec2(0, -10));
    drawNode[2]->setLocalZOrder(5);
    drawNode[2]->drawPolygon(vertices21, sizeof(vertices21) / sizeof(vertices21[0]), Color4F::GREEN, 0.3f, Color4F::YELLOW);
    drawNode[2]->drawPolygon(vertices24, sizeof(vertices24) / sizeof(vertices24[0]), Color4F::BLUE, 0.3f, Color4F::RED);
}

string DrawNodeCocos2dx_Issue1319::title() const
{
    return "GitHub Issue #1319";
}

string DrawNodeCocos2dx_Issue1319::subtitle() const
{
    return "Draw complex FILLED polygons";
}



#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
