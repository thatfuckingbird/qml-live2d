/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppLive2DManager.hpp"
#include <string>
#include <GL/glew.h>
#include <Rendering/CubismRenderer.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppModel.hpp"
#include "LAppView.hpp"
#include "LAppSprite.hpp"
#include <Live2DItemRenderer.h>
#include <QDir>

using namespace Csm;
using namespace LAppDefine;
using namespace std;

LAppLive2DManager::LAppLive2DManager(Live2DItemRenderer* renderer)
    : renderer(renderer),
      _viewMatrix(NULL)
{
    ChangeScene(renderer->getModelPath());
}

LAppLive2DManager::~LAppLive2DManager()
{
    ReleaseModel();
}

void LAppLive2DManager::ReleaseModel()
{
    if(_model)
    {
        delete _model;
        _model = nullptr;
    }
}

LAppModel* LAppLive2DManager::GetModel() const
{
    return _model;
}

void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    if(_model) _model->SetDragging(x, y);
    renderer->dragged(x, y);
}

void LAppLive2DManager::OnTap(bool touchBegin, csmFloat32 x, csmFloat32 y)
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }

    if(renderer && _model)
    {
        QStringList tappedAreas = _model->HitTest(x ,y);
        if(touchBegin)
        {
            renderer->touched(x, y);
            if(!tappedAreas.empty()) renderer->hitAreasTouched(tappedAreas);
        } else
        {
            renderer->tapped(x, y);
            if(!tappedAreas.empty()) renderer->hitAreasTapped(tappedAreas);
        }
    }
}

void LAppLive2DManager::OnUpdate() const
{
    CubismMatrix44 projection;
    int width = renderer->getWidth(), height = renderer->getHeight();
    projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));

    if (_viewMatrix != NULL)
    {
        projection.MultiplyByMatrix(_viewMatrix);
    }

    if(_model)
    {
        const CubismMatrix44    saveProjection = projection;
        projection = saveProjection;

        renderer->getView()->PreModelDraw(*_model);

        _model->Update();
        _model->Draw(projection);///< 参照渡しなのでprojectionは変質する

        renderer->getView()->PostModelDraw(*_model);
    }
}

void LAppLive2DManager::ChangeScene(const QString &modelPath)
{
    if(modelPath.isEmpty() || !QDir{modelPath}.exists()) return;

    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    std::string modelJsonName = QDir{modelPath}.dirName().toStdString();
    modelJsonName += ".model3.json";

    ReleaseModel();
    _model = new LAppModel(renderer);
    _model->LoadAssets((modelPath + "/").toStdString().c_str(), modelJsonName.c_str());

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // LAppViewの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _model = new LAppModel();
        _model->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
        _model->GetModelMatrix()->TranslateX(0.2f);
#endif

        float clearColor[3] = { 1.0f, 1.0f, 1.0f };

        renderer->getView()->SwitchRenderingTarget(useRenderTarget);

        if(useRenderTarget)
        {
            renderer->getView()->SwitchRenderingTarget(useRenderTarget);
            // 背景クリア色
            renderer->getView()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);
        }
    }
}
