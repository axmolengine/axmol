/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "csmRectF.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

csmRectF::csmRectF()
{ }

csmRectF::csmRectF(csmFloat32 x, csmFloat32 y, csmFloat32 w, csmFloat32 h)
    : X(x)
    , Y(y)
    , Width(w)
    , Height(h)
{ }

csmRectF::~csmRectF()
{ }

void csmRectF::SetRect(csmRectF* r)
{
    X = r->X;
    Y = r->Y;
    Width = r->Width;
    Height = r->Height;
}

void csmRectF::Expand(csmFloat32 w, csmFloat32 h)
{
    X -= w;
    Y -= h;
    Width += w * 2.0f;
    Height += h * 2.0f;
}

}}}
//--------- LIVE2D NAMESPACE ------------
