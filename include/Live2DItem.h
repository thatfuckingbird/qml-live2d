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

#include <QQuickFramebufferObject>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QList>

struct MouseEventData
{
    int action;
    int button;
    double x;
    double y;
};

struct MotionRequestData
{
    enum class Type
    {
        StopAll,
        Expression,
        Motion
    };
    Type type;
    QString group;
    QString name;
    int priority;
};

class Live2DItemRenderer;

class Live2DItem : public QQuickFramebufferObject
{
    Q_OBJECT

    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QString backgroundPath READ backgroundPath WRITE setBackgroundPath NOTIFY backgroundPathChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QString randomMotionGroup READ randomMotionGroup WRITE setRandomMotionGroup NOTIFY randomMotionGroupChanged)
    Q_PROPERTY(bool playRandomMotions READ playRandomMotions WRITE setPlayRandomMotions NOTIFY playRandomMotionsChanged)
    Q_PROPERTY(bool blinkingEnabled READ blinkingEnabled WRITE setBlinkingEnabled NOTIFY blinkingEnabledChanged)
    Q_PROPERTY(bool breathingEnabled READ breathingEnabled WRITE setBreathingEnabled NOTIFY breathingEnabledChanged)
    Q_PROPERTY(bool physicsEnabled READ physicsEnabled WRITE setPhysicsEnabled NOTIFY physicsEnabledChanged)
    Q_PROPERTY(bool lipSyncEnabled READ lipSyncEnabled WRITE setLipSyncEnabled NOTIFY lipSyncEnabledChanged)
    Q_PROPERTY(double lipSyncValue READ lipSyncValue WRITE setLipSyncValue NOTIFY lipSyncValueChanged)
    Q_PROPERTY(bool followMouse READ followMouse WRITE setFollowMouse NOTIFY followMouseChanged)
    Q_PROPERTY(bool reactToTouch READ reactToTouch WRITE setReactToTouch NOTIFY reactToTouchChanged)

public:
    Live2DItem(QQuickItem * parent = 0);
    virtual Renderer *createRenderer() const override;
    virtual ~Live2DItem();

    void setModelPath(const QString& modelPath);
    void setBackgroundPath(const QString& backgroundPath);
    void setBackgroundColor(const QColor& backgroundColor);
    QString modelPath() const;
    QString backgroundPath() const;
    QColor backgroundColor() const;
    QString randomMotionGroup() const;
    bool playRandomMotions() const;
    bool blinkingEnabled() const;
    bool breathingEnabled() const;
    bool physicsEnabled() const;
    bool lipSyncEnabled() const;
    double lipSyncValue() const;
    bool followMouse() const;
    bool reactToTouch() const;
    void setRandomMotionGroup(const QString& groupName);
    void setPlayRandomMotions(bool enabled);
    void setBlinkingEnabled(bool enabled);
    void setBreathingEnabled(bool enabled);
    void setPhysicsEnabled(bool enabled);
    void setLipSyncEnabled(bool enabled);
    void setLipSyncValue(double value);
    void setFollowMouse(bool enabled);
    void setReactToTouch(bool enabled);

    Q_INVOKABLE QStringList hitAreas() const;
    Q_INVOKABLE QStringList expressions() const;
    Q_INVOKABLE QStringList motionGroups() const;
    Q_INVOKABLE QStringList motions(const QString& group) const;
    enum Priority
    {
        None = 0,
        Idle,
        Normal,
        Force
    };
    Q_ENUM(Priority)
    Q_INVOKABLE void setExpression(const QString& expression);
    Q_INVOKABLE void startMotion(const QString& motionGroup, const QString& motion, Live2DItem::Priority priority);
    Q_INVOKABLE void stopAllMotions();

    enum class LogLevel // Copied from CubismFramework.hpp, need to keep it in sync with that!
    {
        Verbose = 0,
        Debug,
        Info,
        Warning,
        Error,
        Off
    };
    typedef void (*LogFunction)(const char* message); // Copied from Live2DCubismCore.h, need to keep it in sync with that!
    static LogFunction logFunction;
    static LogLevel logLevel;
    static void defaultLogFunction(const char* message);
    friend class Live2DItemRenderer;

protected:
    virtual void hoverMoveEvent(QHoverEvent *event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void hoverEnterEvent(QHoverEvent* event) override;

signals:
    void modelPathChanged(const QString& modelPath);
    void backgroundPathChanged(const QString& backgroundPath);
    void backgroundColorChanged(const QColor& backgroundColor);
    void tapped(double x, double y);
    void hitAreasTapped(const QStringList& hitAreas);
    void touched(double x, double y);
    void hitAreasTouched(const QStringList& hitAreas);
    void dragged(double x, double y);
    void modelReady();
    void randomMotionGroupChanged(const QString&);
    void playRandomMotionsChanged(bool);
    void blinkingEnabledChanged(bool);
    void breathingEnabledChanged(bool);
    void physicsEnabledChanged(bool);
    void lipSyncEnabledChanged(bool);
    void lipSyncValueChanged(double);
    void motionFinished(bool null, const QString& motionGroup, const QString& motion);
    void reactToTouchChanged(bool);
    void followMouseChanged(bool);

private:
    QString m_modelPath;
    QString m_backgroundPath;
    QColor m_backgroundColor = Qt::transparent;
    bool m_modelPathChanged = false;
    bool m_backgroundPathChanged = false;
    QVector<MouseEventData> m_mouseEventQueue;
    QVector<MotionRequestData> m_motionRequestQueue;
    mutable Live2DItemRenderer* m_renderer = nullptr;
    bool m_playRandomMotions = false;
    QString m_randomMotionGroup = "Idle";
    bool m_enableBlink = true;
    bool m_enableBreath = true;
    bool m_enablePhysics = true;
    bool m_enableLipSync = false;
    double m_lipSyncValue = 0;
    bool m_followMouse = true;
    bool m_reactToTouch = true;
    bool m_mousePressed = false;
};
