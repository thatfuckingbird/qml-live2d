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

#include <Live2DItem.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <GL/glew.h>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include <QMouseEvent>
#include <GL/glew.h>
#include "Type/csmVector.hpp"
#include "LAppAllocator.hpp"
#include <Live2DItemRenderer.h>
#include <QQuickWindow>

using namespace Csm;
using namespace LAppDefine;

Live2DItem::LogLevel Live2DItem::logLevel = Live2DItem::LogLevel::Off;
Live2DItem::LogFunction Live2DItem::logFunction = &Live2DItem::defaultLogFunction;

Live2DItem::Live2DItem(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
    setMirrorVertically(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

void Live2DItem::setModelPath(const QString &modelPath)
{
    if(modelPath != m_modelPath)
    {
        m_modelPath = modelPath;
        m_modelPathChanged = true;
        update();
        emit this->modelPathChanged(modelPath);
    }
}

void Live2DItem::setBackgroundPath(const QString &backgroundPath)
{
    if(backgroundPath != m_backgroundPath)
    {
        m_backgroundPath = backgroundPath;
        m_backgroundPathChanged = true;
        update();
        emit this->backgroundPathChanged(backgroundPath);
    }
}

void Live2DItem::setBackgroundColor(const QColor &backgroundColor)
{
    if(backgroundColor != m_backgroundColor)
    {
        m_backgroundColor = backgroundColor;
        update();
        emit this->backgroundColorChanged(backgroundColor);
    }
}

QString Live2DItem::modelPath() const
{
    return m_modelPath;
}

QString Live2DItem::backgroundPath() const
{
    return m_backgroundPath;
}

QColor Live2DItem::backgroundColor() const
{
    return m_backgroundColor;
}

QString Live2DItem::randomMotionGroup() const
{
    return m_randomMotionGroup;
}

bool Live2DItem::playRandomMotions() const
{
    return m_playRandomMotions;
}

bool Live2DItem::blinkingEnabled() const
{
    return m_enableBlink;
}

bool Live2DItem::breathingEnabled() const
{
    return m_enableBreath;
}

bool Live2DItem::physicsEnabled() const
{
    return m_enablePhysics;
}

bool Live2DItem::lipSyncEnabled() const
{
    return m_enableLipSync;
}

double Live2DItem::lipSyncValue() const
{
    return m_lipSyncValue;
}

bool Live2DItem::followMouse() const
{
    return m_followMouse;
}

bool Live2DItem::reactToTouch() const
{
    return m_reactToTouch;
}

void Live2DItem::setRandomMotionGroup(const QString &groupName)
{
    if(m_randomMotionGroup != groupName) {
        m_randomMotionGroup = groupName;
        emit randomMotionGroupChanged(groupName);
        update();
    }
}

void Live2DItem::setPlayRandomMotions(bool enabled)
{
    if(m_playRandomMotions != enabled) {
        m_playRandomMotions = enabled;
        emit playRandomMotionsChanged(enabled);
        update();
    }
}

void Live2DItem::setBlinkingEnabled(bool enabled)
{
    if(m_enableBlink != enabled) {
        m_enableBlink = enabled;
        emit blinkingEnabledChanged(enabled);
        update();
    }
}

void Live2DItem::setBreathingEnabled(bool enabled)
{
    if(m_enableBreath != enabled) {
        m_enableBreath = enabled;
        emit breathingEnabledChanged(enabled);
        update();
    }
}

void Live2DItem::setPhysicsEnabled(bool enabled)
{
    if(m_enablePhysics != enabled) {
        m_enablePhysics = enabled;
        emit physicsEnabledChanged(enabled);
        update();
    }
}

void Live2DItem::setLipSyncEnabled(bool enabled)
{
    if(m_enableLipSync != enabled) {
        m_enableLipSync = enabled;
        emit lipSyncEnabledChanged(enabled);
        update();
    }
}

void Live2DItem::setLipSyncValue(double value)
{
    if(m_lipSyncValue != value) {
        m_lipSyncValue = value;
        emit lipSyncValueChanged(value);
        update();
    }
}

void Live2DItem::setFollowMouse(bool enabled)
{
    if(m_followMouse != enabled) {
        m_followMouse = enabled;
        emit followMouseChanged(enabled);
        update();
    }
}

void Live2DItem::setReactToTouch(bool enabled)
{
    if(m_reactToTouch != enabled) {
        m_reactToTouch = enabled;
        emit reactToTouchChanged(enabled);
        update();
    }
}

QStringList Live2DItem::hitAreas() const
{
    // TODO: This is not thread-safe
    if(m_renderer) return m_renderer->hitAreaNames();
    return {};
}

QStringList Live2DItem::expressions() const
{
    // TODO: This is not thread-safe
    if(m_renderer) return m_renderer->expressionNames();
    return {};
}

QStringList Live2DItem::motionGroups() const
{
    if(m_renderer) return m_renderer->motionGroupNames();
    return {};
}

QStringList Live2DItem::motions(const QString &group) const
{
    if(m_renderer) return m_renderer->motionNames(group);
    return {};
}

void Live2DItem::setMouseFollowPosition(double x, double y, bool force)
{
    if(!force) {
        if(!m_followMouse && !m_reactToTouch) return;
        if(!m_followMouse && !m_mousePressed) return;
    }
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 x,
                                 y,
                                 true
                             });
    update();
}

void Live2DItem::resetMouseFollowPosition()
{
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 width()/2.0,
                                 height()/2.0,
                                 true
                             });
    update();
}

void Live2DItem::setExpression(const QString &expressionName)
{
    m_motionRequestQueue.append({
                                    MotionRequestData::Type::Expression,
                                    {},
                                    expressionName,
                                    {}
                                });
    update();
}

void Live2DItem::startMotion(const QString &motionGroup, const QString &motion, Live2DItem::Priority priority)
{
    m_motionRequestQueue.append({
                                    MotionRequestData::Type::Motion,
                                    motionGroup,
                                    motion,
                                    priority
                                });
    update();
}

void Live2DItem::stopAllMotions()
{
    m_motionRequestQueue.append({
                                    MotionRequestData::Type::StopAll,
                                    {},
                                    {},
                                    {}
                                });
    update();
}

void Live2DItem::defaultLogFunction(const char *message)
{
    std::cerr << message << std::endl;
}

Live2DItem::~Live2DItem()
{

}

QQuickFramebufferObject::Renderer *Live2DItem::createRenderer() const
{
    window()->setPersistentOpenGLContext(true);
    auto renderer = new Live2DItemRenderer(const_cast<Live2DItem*>(this));

    if(m_renderer) {
        disconnect(m_renderer, &QObject::destroyed, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::hitAreasTapped, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::tapped, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::touched, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::hitAreasTouched, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::cursorMoved, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::modelReady, this, nullptr);
        disconnect(m_renderer, &Live2DItemRenderer::motionFinished, this, nullptr);
    }
    connect(renderer, &QObject::destroyed, this, [&](){
        m_renderer = nullptr;
    });
    connect(renderer, &Live2DItemRenderer::hitAreasTapped, this, &Live2DItem::hitAreasTapped);
    connect(renderer, &Live2DItemRenderer::tapped, this, &Live2DItem::tapped);
    connect(renderer, &Live2DItemRenderer::touched, this, &Live2DItem::touched);
    connect(renderer, &Live2DItemRenderer::hitAreasTouched, this, &Live2DItem::hitAreasTouched);
    connect(renderer, &Live2DItemRenderer::cursorMoved, this, &Live2DItem::cursorMoved);
    connect(renderer, &Live2DItemRenderer::modelReady, this, &Live2DItem::modelReady);
    connect(renderer, &Live2DItemRenderer::motionFinished, this, &Live2DItem::motionFinished);

    m_renderer = renderer;
    return renderer;
}

void Live2DItem::hoverMoveEvent(QHoverEvent *event)
{
    if(!m_followMouse && !m_reactToTouch) return;
    if(!m_followMouse && !m_mousePressed) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 event->posF().x(),
                                 event->posF().y(),
                                 false
                             });
    update();
}

void Live2DItem::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    if(event->button() != Qt::LeftButton || !m_reactToTouch) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseButtonPress,
                                 Qt::LeftButton,
                                 0,
                                 0,
                                 false
                             });
    update();
}

void Live2DItem::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressed = false;
    if(event->button() != Qt::LeftButton || !m_reactToTouch) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseButtonRelease,
                                 Qt::LeftButton,
                                 0,
                                 0,
                                 false
                             });
    update();
}

void Live2DItem::hoverEnterEvent(QHoverEvent *event)
{
    if(!m_followMouse && !m_reactToTouch) return;
    if(!m_followMouse && !m_mousePressed) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 event->posF().x(),
                                 event->posF().y(),
                                 false
                             });
    update();
}
