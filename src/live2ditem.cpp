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

#include "live2ditem.h"
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
#include "live2ditemrenderer.h"
#include <QQuickWindow>

using namespace Csm;
using namespace LAppDefine;

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

Live2DItem::~Live2DItem()
{

}

QQuickFramebufferObject::Renderer *Live2DItem::createRenderer() const
{
    window()->setPersistentOpenGLContext(true);
    //window()->setPersistentSceneGraph(true);
    auto renderer = new Live2DItemRenderer(const_cast<Live2DItem*>(this));
    return renderer;
}

void Live2DItem::hoverMoveEvent(QHoverEvent *event)
{
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 event->posF().x(),
                                 event->posF().y()
                             });
    update();
}

void Live2DItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseButtonPress,
                                 Qt::LeftButton,
                                 0,
                                 0
                             });
    update();
}

void Live2DItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton) return;
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseButtonRelease,
                                 Qt::LeftButton,
                                 0,
                                 0
                             });
    update();
}

void Live2DItem::hoverEnterEvent(QHoverEvent *event)
{
    m_mouseEventQueue.append({
                                 QMouseEvent::MouseMove,
                                 -1,
                                 event->posF().x(),
                                 event->posF().y()
                             });
    update();
}
