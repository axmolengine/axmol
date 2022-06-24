/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismTargetPoint.hpp"
#include "Math/CubismMath.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

const csmInt32      FrameRate = 30;
const csmFloat32    Epsilon = 0.01f;

CubismTargetPoint::CubismTargetPoint()
                                    : _faceTargetX(0.0f)
                                    , _faceTargetY(0.0f)
                                    , _faceX(0.0f)
                                    , _faceY(0.0f)
                                    , _faceVX(0.0f)
                                    , _faceVY(0.0f)
                                    , _lastTimeSeconds(0.0f)
                                    , _userTimeSeconds(0.0f)
{ }

CubismTargetPoint::~CubismTargetPoint()
{ }

void CubismTargetPoint::Update(csmFloat32 deltaTimeSeconds)
{
    // デルタ時間を加算する
    _userTimeSeconds += deltaTimeSeconds;

    // 首を中央から左右に振るときの平均的な早さは  秒程度。加速・減速を考慮して、その2倍を最高速度とする
    // 顔のふり具合を、中央(0.0)から、左右は(+-1.0)とする
    const csmFloat32 FaceParamMaxV = 40.0 / 10.0f;                                      // 7.5秒間に40分移動（5.3/sc)
    const csmFloat32 MaxV = FaceParamMaxV * 1.0f / static_cast<csmFloat32>(FrameRate);  // 1frameあたりに変化できる速度の上限

    if (_lastTimeSeconds == 0.0f)
    {
        _lastTimeSeconds = _userTimeSeconds;
        return;
    }

    const csmFloat32  deltaTimeWeight = (_userTimeSeconds - _lastTimeSeconds) * static_cast<csmFloat32>(FrameRate);
    _lastTimeSeconds = _userTimeSeconds;

    // 最高速度になるまでの時間を
    const csmFloat32 TimeToMaxSpeed = 0.15f;
    const csmFloat32 FrameToMaxSpeed = TimeToMaxSpeed * static_cast<csmFloat32>(FrameRate);     // sec * frame/sec
    const csmFloat32 MaxA = deltaTimeWeight * MaxV / FrameToMaxSpeed;                           // 1frameあたりの加速度

    // 目指す向きは、(dx, dy)方向のベクトルとなる
    const csmFloat32 dx = _faceTargetX - _faceX;
    const csmFloat32 dy = _faceTargetY - _faceY;

    if (CubismMath::AbsF(dx) <= Epsilon && CubismMath::AbsF(dy) <= Epsilon)
    {
        return; // 変化なし
    }

    // 速度の最大よりも大きい場合は、速度を落とす
    const csmFloat32 d = CubismMath::SqrtF((dx * dx) + (dy * dy));

    // 進行方向の最大速度ベクトル
    const csmFloat32 vx = MaxV * dx / d;
    const csmFloat32 vy = MaxV * dy / d;

    // 現在の速度から、新規速度への変化（加速度）を求める
    csmFloat32 ax = vx - _faceVX;
    csmFloat32 ay = vy - _faceVY;

    const csmFloat32 a = CubismMath::SqrtF((ax * ax) + (ay * ay));

    // 加速のとき
    if (a < -MaxA || a > MaxA)
    {
        ax *= MaxA / a;
        ay *= MaxA / a;
    }

    // 加速度を元の速度に足して、新速度とする
    _faceVX += ax;
    _faceVY += ay;

    // 目的の方向に近づいたとき、滑らかに減速するための処理
    // 設定された加速度で止まることのできる距離と速度の関係から
    // 現在とりうる最高速度を計算し、それ以上のときは速度を落とす
    // ※本来、人間は筋力で力（加速度）を調整できるため、より自由度が高いが、簡単な処理ですませている
    {
        // 加速度、速度、距離の関係式。
        //            2  6           2               3
        //      sqrt(a  t  + 16 a h t  - 8 a h) - a t
        // v = --------------------------------------
        //                    2
        //                 4 t  - 2
        // (t=1)
        //  時刻tは、あらかじめ加速度、速度を1/60(フレームレート、単位なし)で
        //  考えているので、t＝１として消してよい（※未検証）

        const csmFloat32 maxV = 0.5f * (CubismMath::SqrtF((MaxA * MaxA) + 16.0f * MaxA * d - 8.0f * MaxA * d) - MaxA);
        const csmFloat32 curV = CubismMath::SqrtF((_faceVX * _faceVX) + (_faceVY * _faceVY));

        if (curV > maxV)
        {
            // 現在の速度 > 最高速度のとき、最高速度まで減速
            _faceVX *= maxV / curV;
            _faceVY *= maxV / curV;
        }
    }

    _faceX += _faceVX;
    _faceY += _faceVY;
}

void CubismTargetPoint::Set(csmFloat32 x, csmFloat32 y)
{
    this->_faceTargetX = x;
    this->_faceTargetY = y;
}

csmFloat32 CubismTargetPoint::GetX() const
{
    return this->_faceX;
}

csmFloat32 CubismTargetPoint::GetY() const
{
    return this->_faceY;
}

}}}
