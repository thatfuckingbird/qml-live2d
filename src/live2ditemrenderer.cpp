/*
qml-live2d
Copyright (C) 2020  thatfuckingbird

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "LAppAllocator.hpp"
#include "LAppModel.hpp"
#include <Live2DItem.h>
#include <Live2DItemRenderer.h>
#include <QQuickWindow>

using namespace Live2D::Cubism::Framework;

bool Live2DItemRenderer::m_frameworkInitialized = false;

float Live2DTimerData::getDeltaTime()
{
    return static_cast<csmFloat32>(m_deltaTime);
}

void Live2DTimerData::updateTime()
{
    if(!m_startTimeSet)
    {
        m_startTime = std::chrono::high_resolution_clock::now();
        m_startTimeSet = true;
    }

    m_currentFrame = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_startTime)).count();
    m_deltaTime = m_currentFrame - m_lastFrame;
    m_lastFrame = m_currentFrame;
}

Live2DItemRenderer::Live2DItemRenderer(Live2DItem* item) : m_item{item}, m_captured(false), m_mouseX(0.0f), m_mouseY(0.0f), m_windowWidth(0), m_windowHeight(0)
{
    m_view = new LAppView{this};
    m_textureManager = new LAppTextureManager();

    if (glewInit() != GLEW_OK) {
        if (LAppDefine::DebugLogEnable)
        {
            LAppPal::PrintLog("Can't initilize glew.");
        }
    }

    m_view->Initialize();

    initializeCubism();

    m_timerData.updateTime();

    //load model
    m_appManager = new LAppLive2DManager{this};

    m_view->InitializeSprite({}, {});
}

void Live2DItemRenderer::initializeCubism()
{
    static LAppAllocator _cubismAllocator;
    static Csm::CubismFramework::Option _cubismOption;

    if(!m_frameworkInitialized)
    {
        m_frameworkInitialized = true;

        LAppDefine::DebugLogEnable = Live2DItem::logLevel == Live2DItem::LogLevel::Verbose || Live2DItem::logLevel == Live2DItem::LogLevel::Debug;
        LAppDefine::DebugTouchLogEnable = Live2DItem::logLevel == Live2DItem::LogLevel::Verbose;

        _cubismOption.LogFunction = Live2DItem::logFunction;
        _cubismOption.LoggingLevel = static_cast<CubismFramework::Option::LogLevel>(Live2DItem::logLevel);
        Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

        CubismFramework::Initialize();
    }
}

GLuint Live2DItemRenderer::createShader() const
{
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader =
            "#version 120\n"
            "attribute vec3 position;"
            "attribute vec2 uv;"
            "varying vec2 vuv;"
            "void main(void){"
            "    gl_Position = vec4(position, 1.0);"
            "    vuv = uv;"
            "}";
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader =
            "#version 120\n"
            "varying vec2 vuv;"
            "uniform sampler2D texture;"
            "uniform vec4 baseColor;"
            "void main(void){"
            "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
            "}";
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

QStringList Live2DItemRenderer::hitAreaNames() const
{
    if(auto model = m_appManager->GetModel()) return model->HitAreaNames();
    return {};
}

QStringList Live2DItemRenderer::expressionNames() const
{
    if(auto model = m_appManager->GetModel()) return model->ExpressionNames();
    return {};
}

QStringList Live2DItemRenderer::motionGroupNames() const
{
    if(auto model = m_appManager->GetModel()) return model->MotionGroupNames();
    return {};
}

QStringList Live2DItemRenderer::motionNames(const QString &group) const
{
    if(auto model = m_appManager->GetModel()) return model->MotionNames(group);
    return {};
}

int Live2DItemRenderer::getWidth() const
{
    return m_windowWidth;
}

int Live2DItemRenderer::getHeight() const
{
    return m_windowHeight;
}

QString Live2DItemRenderer::getModelPath() const
{
    return m_modelPath;
}

LAppView *Live2DItemRenderer::getView() const
{
    return m_view;
}

Live2DTimerData& Live2DItemRenderer::getTimerData()
{
    return m_timerData;
}

LAppTextureManager *Live2DItemRenderer::getTextureManager() const
{
    return m_textureManager;
}

LAppLive2DManager *Live2DItemRenderer::getAppManager() const
{
    return m_appManager;
}

Live2DItemRenderer::~Live2DItemRenderer()
{
    delete m_textureManager;
    delete m_view;
    delete m_appManager;
}

//This function is called when a new FBO is needed.
//This happens on the initial frame or when the item is resized.
QOpenGLFramebufferObject * Live2DItemRenderer::createFramebufferObject(const QSize &size)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_windowWidth = size.width();
    m_windowHeight = size.height();

    m_view->Initialize();
    m_view->ResizeSprite();

    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}

void Live2DItemRenderer::render()
{
    m_timerData.updateTime();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    m_view->Render();
    m_item->window()->resetOpenGLState();
    update();
}

void Live2DItemRenderer::synchronize(QQuickFramebufferObject *item)
{
    Live2DItem* l2d = dynamic_cast<Live2DItem*>(item);
    m_backgroundColor = l2d->backgroundColor();
    if(l2d->m_backgroundPathChanged || l2d->m_modelPathChanged)
    {
        m_modelPath = l2d->m_modelPath;
        m_backgroundPath = l2d->m_backgroundPath;

        l2d->m_backgroundPathChanged = false;
        l2d->m_modelPathChanged = false;

        m_appManager->ChangeScene(m_modelPath);
        delete m_view;
        m_view = new LAppView{this};
        m_textureManager = new LAppTextureManager();
        m_view->Initialize();
        m_view->InitializeSprite(m_modelPath, m_backgroundPath);

        emit modelReady();
    }
    for(int i = 0; i < l2d->m_mouseEventQueue.size(); i++)
    {
        processMouseEvent(&l2d->m_mouseEventQueue.at(i));
    }
    l2d->m_mouseEventQueue.clear();
    if(auto model = m_appManager->GetModel()) {
        model->playRandomMotions = l2d->m_playRandomMotions;
        model->randomMotionGroup = l2d->m_randomMotionGroup.toStdString().c_str();
        model->enableBlink = l2d->m_enableBlink;
        model->enableBreath = l2d->m_enableBreath;
        model->enablePhysics = l2d->m_enablePhysics;
        model->enableLipSync = l2d->m_enableLipSync;
        model->lipSyncValue = l2d->m_lipSyncValue;
    }
    for(int i = 0; i < l2d->m_motionRequestQueue.size(); i++)
    {
        processMotionRequest(&l2d->m_motionRequestQueue.at(i));
    }
    l2d->m_motionRequestQueue.clear();
}

void Live2DItemRenderer::processMouseEvent(const MouseEventData* event)
{
    if(!m_view) return;

    if(event->action == QMouseEvent::MouseButtonPress && event->button == Qt::LeftButton)
    {
        m_captured = true;
        m_view->OnTouchesBegan(m_mouseX, m_mouseY);
    } else if(event->action == QMouseEvent::MouseButtonRelease && event->button == Qt::LeftButton)
    {
        if(m_captured)
        {
            m_captured = false;
            m_view->OnTouchesEnded(m_mouseX, m_mouseY);
        }
    } else if(event->action == QMouseEvent::MouseMove)
    {
        m_mouseX = event->x;
        m_mouseY = event->y;
        m_view->OnTouchesMoved(m_mouseX, m_mouseY, event->override);
    }
}

void Live2DItemRenderer::processMotionRequest(const MotionRequestData *request)
{
    auto model = m_appManager->GetModel();
    if(!model) return;

    if(request->type == MotionRequestData::Type::StopAll)
    {
        model->StopAllMotions();
    } else if(request->type == MotionRequestData::Type::Expression)
    {
        model->SetExpression(request->name.toStdString().c_str());
    } else if(request->type == MotionRequestData::Type::Motion) {
        model->StartMotion(request->group.toStdString().c_str(), request->name.toStdString().c_str(), request->priority);
    }
}
