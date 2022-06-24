/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismUserModel.hpp"
#include "Motion/CubismMotion.hpp"
#include "Physics/CubismPhysics.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismUserModel::CubismUserModel()
    : _moc(NULL)
    , _model(NULL)
    , _motionManager(NULL)
    , _expressionManager(NULL)
    , _eyeBlink(NULL)
    , _breath(NULL)
    , _modelMatrix(NULL)
    , _pose(NULL)
    , _dragManager(NULL)
    , _physics(NULL)
    , _modelUserData(NULL)
    , _initialized(false)
    , _updating(false)
    , _opacity(1.0f)
    , _lipSync(true)
    , _lastLipSyncValue(0.0f)
    , _dragX(0.0f)
    , _dragY(0.0f)
    , _accelerationX(0.0f)
    , _accelerationY(0.0f)
    , _accelerationZ(0.0f)
    , _debugMode(false)
    , _renderer(NULL)
{
    // モーションマネージャーを作成
    // MotionQueueManagerクラスからの継承なので使い方は同じ
    _motionManager = CSM_NEW CubismMotionManager();
    _motionManager->SetEventCallback(CubismDefaultMotionEventCallback, this);

    // 表情モーションマネージャを作成
    _expressionManager = CSM_NEW CubismMotionManager();

    // ドラッグによるアニメーション
    _dragManager = CSM_NEW CubismTargetPoint();

}

CubismUserModel::~CubismUserModel()
{
    CSM_DELETE(_motionManager);
    CSM_DELETE(_expressionManager);
    if (_moc)
    {
        _moc->DeleteModel(_model);
    }
    CubismMoc::Delete(_moc);
    CSM_DELETE(_modelMatrix);
    CubismPose::Delete(_pose);
    CubismEyeBlink::Delete(_eyeBlink);
    CubismBreath::Delete(_breath);
    CSM_DELETE(_dragManager);
    CubismPhysics::Delete(_physics);
    CubismModelUserData::Delete(_modelUserData);

    DeleteRenderer();
}

void CubismUserModel::SetAcceleration(csmFloat32 x, csmFloat32 y, csmFloat32 z)
{
    _accelerationX = x;
    _accelerationY = y;
    _accelerationZ = z;
}

void CubismUserModel::LoadModel(const csmByte* buffer, csmSizeInt size)
{
    _moc = CubismMoc::Create(buffer, size);

    if (_moc == NULL)
    {
        CubismLogError("Failed to CubismMoc::Create().");
        return;
    }

    _model = _moc->CreateModel();

    if (_model == NULL)
    {
        CubismLogError("Failed to CreateModel().");
        return;
    }

    _model->SaveParameters();
    _modelMatrix = CSM_NEW CubismModelMatrix(_model->GetCanvasWidth(), _model->GetCanvasHeight());

}

ACubismMotion* CubismUserModel::LoadExpression(const csmByte* buffer, csmSizeInt size, const csmChar* name)
{
    return CubismExpressionMotion::Create(buffer, size);
}

void CubismUserModel::LoadPose(const csmByte* buffer, csmSizeInt size)
{
    _pose = CubismPose::Create(buffer, size);
}

void CubismUserModel::LoadPhysics(const csmByte* buffer, csmSizeInt size)
{
    _physics = CubismPhysics::Create(buffer, size);
}

void CubismUserModel::LoadUserData(const csmByte* buffer, csmSizeInt size)
{
    _modelUserData = CubismModelUserData::Create(buffer, size);
}
csmBool CubismUserModel::IsHit(CubismIdHandle drawableId, csmFloat32 pointX, csmFloat32 pointY)
{
    const csmInt32 drawIndex = _model->GetDrawableIndex(drawableId);

    if (drawIndex < 0)
    {
        return false; // 存在しない場合はfalse
    }

    const csmInt32    count = _model->GetDrawableVertexCount(drawIndex);
    const csmFloat32* vertices = _model->GetDrawableVertices(drawIndex);

    csmFloat32 left = vertices[0];
    csmFloat32 right = vertices[0];
    csmFloat32 top = vertices[1];
    csmFloat32 bottom = vertices[1];

    for (csmInt32 j = 1; j < count; ++j)
    {
        csmFloat32 x = vertices[Constant::VertexOffset + j * Constant::VertexStep];
        csmFloat32 y = vertices[Constant::VertexOffset + j * Constant::VertexStep + 1];

        if (x < left)
        {
            left = x; // Min x
        }

        if (x > right)
        {
            right = x; // Max x
        }

        if (y < top)
        {
            top = y; // Min y
        }

        if (y > bottom)
        {
            bottom = y; // Max y
        }
    }

    const csmFloat32 tx = _modelMatrix->InvertTransformX(pointX);
    const csmFloat32 ty = _modelMatrix->InvertTransformY(pointY);

    return ((left <= tx) && (tx <= right) && (top <= ty) && (ty <= bottom));
}

ACubismMotion* CubismUserModel::LoadMotion(const csmByte* buffer, csmSizeInt size, const csmChar* name, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    return CubismMotion::Create(buffer, size, onFinishedMotionHandler);
}

void CubismUserModel::SetDragging(csmFloat32 x, csmFloat32 y)
{
    _dragManager->Set(x, y);
}

CubismModelMatrix* CubismUserModel::GetModelMatrix() const
{
    return _modelMatrix;
}

csmBool CubismUserModel::IsInitialized()
{
    return _initialized;
}

void CubismUserModel::IsInitialized(csmBool v)
{
    _initialized = v;
}

csmBool CubismUserModel::IsUpdating()
{
    return _updating;
}

void CubismUserModel::IsUpdating(csmBool v)
{
    _updating = v;
}

void CubismUserModel::SetOpacity(csmFloat32 a)
{
    _opacity = a;
}

csmFloat32 CubismUserModel::GetOpacity()
{
    return _opacity;
}

CubismModel* CubismUserModel::GetModel() const
{
    return _model;
}

void CubismUserModel::CreateRenderer()
{
    if (_renderer)
    {
        DeleteRenderer();
    }
    _renderer = Rendering::CubismRenderer::Create();

    _renderer->Initialize(_model);
}

void CubismUserModel::DeleteRenderer()
{
    if (_renderer)
    {
        Rendering::CubismRenderer::Delete(_renderer);

        _renderer = NULL;
    }
}

void CubismUserModel::CubismDefaultMotionEventCallback(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData)
{
    CubismUserModel* model = reinterpret_cast<CubismUserModel*>(customData);
    if (model != NULL)
    {
        model->MotionEventFired(eventValue);
    }
}

void CubismUserModel::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s",eventValue.GetRawString());
}

}}}
