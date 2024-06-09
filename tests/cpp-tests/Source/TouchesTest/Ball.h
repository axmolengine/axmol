/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef _BALL_H_
#define _BALL_H_

#include "axmol.h"

class Paddle;

class Ball : public ax::Sprite
{
    ax::Vec2 _velocity;

public:
    Ball();
    virtual ~Ball();

    float radius();
    // BOOL initWithTexture(ax::Texture2D* aTexture);
    // virtual void setTexture(ax::Texture2D* newTexture);
    void move(float delta);
    void collideWithPaddle(Paddle* paddle);

public:
    void setVelocity(ax::Vec2 velocity) { _velocity = velocity; }
    ax::Vec2 getVelocity() { return _velocity; }

public:
    static Ball* ballWithTexture(ax::Texture2D* aTexture);
};

#endif
