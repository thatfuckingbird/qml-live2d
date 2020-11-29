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

class Live2DItem : public QQuickFramebufferObject
{
    Q_OBJECT

    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QString backgroundPath READ backgroundPath WRITE setBackgroundPath NOTIFY backgroundPathChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
    Live2DItem(QQuickItem * parent = 0);
    void setModelPath(const QString& modelPath);
    void setBackgroundPath(const QString& backgroundPath);
    void setBackgroundColor(const QColor& backgroundColor);
    QString modelPath() const;
    QString backgroundPath() const;
    QColor backgroundColor() const;
    virtual Renderer *createRenderer() const override;
    virtual ~Live2DItem();
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

private:
    QString m_modelPath;
    QString m_backgroundPath;
    QColor m_backgroundColor = Qt::transparent;
    bool m_modelPathChanged = false;
    bool m_backgroundPathChanged = false;
    QList<MouseEventData> m_mouseEventQueue;
};
