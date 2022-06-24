/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "cocos2d.h"

class TouchManager
{
private:
    float startY; // タッチを開始した位置
    float startX;

    float lastX; // シングルタッチ時のxの値
    float lastY; // シングルタッチ時のyの値
    float lastX1; // ダブルタッチ時の一つ目のxの値
    float lastY1; // ダブルタッチ時の一つ目のyの値
    float lastX2; // ダブルタッチ時の二つ目のxの値
    float lastY2; // ダブルタッチ時の二つ目のyの値

    float lastTouchDist; // 2本以上でタッチしたときの指の距離

    float deltaX; // 前回の値から今回の値へのxの移動距離。
    float deltaY; // 前回の値から今回の値へのyの移動距離。

    float scale; // このフレームで掛け合わせる拡大率。拡大操作中以外は1。

    bool touchSingle; // シングルタッチ時はtrue
    bool flipAvailable; // フリップが有効かどうか

public:
    TouchManager();
    float getCenterX() { return lastX; }
    float getCenterY() { return lastY; }
    float getDeltaX() { return deltaX; }
    float getDeltaY() { return deltaY; }
    float getStartX() { return startX; }
    float getStartY() { return startY; }
    float getScale() { return scale; }
    float getX() { return lastX; }
    float getY() { return lastY; }
    float getX1() { return lastX1; }
    float getY1() { return lastY1; }
    float getX2() { return lastX2; }
    float getY2() { return lastY2; }
    bool isSingleTouch() { return touchSingle; }
    bool isFlickAvailable() { return flipAvailable; }
    void disableFlick() { flipAvailable = false; }

    void touchesBegan(float deviceX, float deviceY);
    void touchesBegan(float deviceX1, float deviceY1, float deviceX2, float deviceY2);
    void touchesMoved(cocos2d::Point touches[], int num);
    void touchesMoved(float deviceX, float deviceY);
    void touchesMoved(float deviceX1, float deviceY1, float deviceX2, float deviceY2);
    float getFlickDistance();

private:
    float distance(float x1, float y1, float x2, float y2);
    float calcShift(float v1, float v2);
};
