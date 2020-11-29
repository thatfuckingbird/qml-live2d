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

#pragma once

#include <QMouseEvent>
#include <QOpenGLFramebufferObject>
#include <QQuickFramebufferObject>
#include <chrono>

class Live2DItem;
class MouseEventData;
class LAppView;
class LAppTextureManager;
class LAppLive2DManager;

class Live2DTimerData
{
public:
    float getDeltaTime();
    void updateTime();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> m_startTime{};
    bool m_startTimeSet = false;
    double m_currentFrame = 0;
    double m_lastFrame = 0;
    double m_deltaTime = 0;
};

class Live2DItemRenderer : public QQuickFramebufferObject::Renderer
{
public:
    Live2DItemRenderer(Live2DItem* item);
    int getWidth() const;
    int getHeight() const;
    QString getModelPath() const;
    LAppView* getView() const;
    Live2DTimerData& getTimerData();
    LAppTextureManager* getTextureManager() const;
    LAppLive2DManager* getAppManager() const;
    GLuint createShader() const;
    virtual QOpenGLFramebufferObject * createFramebufferObject(const QSize &size) override;
    virtual void render() override;
    virtual void synchronize(QQuickFramebufferObject* item) override;
    virtual ~Live2DItemRenderer();

private:
    static void initializeCubism();
    void processMouseEvent(const MouseEventData* event);

    Live2DItem *m_item = nullptr;
    LAppView* m_view;
    Live2DTimerData m_timerData;
    bool m_captured;
    float m_mouseX;
    float m_mouseY;
    LAppTextureManager* m_textureManager;
    LAppLive2DManager* m_appManager;
    int m_windowWidth;
    int m_windowHeight;
    QString m_modelPath;
    QString m_backgroundPath;
    QColor m_backgroundColor;

    static bool m_frameworkInitialized;
};
