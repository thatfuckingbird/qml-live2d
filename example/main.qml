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

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import Live2D 1.0

Window {
    flags: Qt.WA_TranslucentBackground
    color: "transparent"

    Pane {
        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            Live2DItem {
                id: live2D
                width: 1000
                height: 1000
                modelPath: ""
                backgroundColor: "gray"
                onTapped: logText.append(`Tapped at ${x};${y}`)
                onHitAreasTapped: logText.append(`Hit areas ${hitAreas} tapped`)
                onTouched: logText.append(`Touched at ${x};${y}`)
                onHitAreasTouched: logText.append(`Hit areas ${hitAreas} touched`)
                onModelReady: {
                    logText.append('Model ready')
                    expressionsCombo.model = expressions()
                    let groups = motionGroups()
                    let motionList = []
                    for(let i = 0; i < groups.length; ++i) {
                        let m = motions(groups[i])
                        for(let j = 0; j < m.length; ++j) {
                            motionList.push(`${groups[i]}::${m[j]}`)
                        }
                    }
                    motionsCombo.model = motionList
                }
                onMotionFinished: logText.append(`Motion finished ${motionGroup}/${motion}, success: ${isNull}`)
                playRandomMotions: checkBoxRandomIdle.checked
                reactToTouch: checkBoxReact.checked
                followMouse: checkBoxFollow.checked
                breathingEnabled: checkBoxBreath.checked
                lipSyncEnabled: checkBoxLip.checked
                physicsEnabled: checkBoxPhysics.checked
                blinkingEnabled: checkBoxBlink.checked
            }


            ColumnLayout {
                width: 600
                RowLayout {
                    Label {
                        text: "Model:"
                    }
                    ComboBox {
                        model: ["", "data/Hiyori", "data/lafei_4"]
                        onCurrentTextChanged: {
                            live2D.modelPath = currentText
                        }
                    }
                }
                RowLayout {
                    Label {
                        text: "Expressions:"
                    }
                    ComboBox {
                        id: expressionsCombo
                        Layout.fillWidth: true
                        model: []
                    }
                    Button {
                        text: "Set"
                        onClicked: live2D.setExpression(expressionsCombo.currentText)
                    }
                }
                RowLayout {
                    Label {
                        text: "Motions:"
                    }
                    ComboBox {
                        id: motionsCombo
                        Layout.fillWidth: true
                        model: []
                    }
                    Button {
                        text: "Start"
                        onClicked: {
                            let split = motionsCombo.currentText.split("::")
                            live2D.startMotion(split[0],split[1],Live2DItem.Force)
                        }
                    }
                    Button {
                        text: "Stop all"
                        onClicked: live2D.stopAllMotions()
                    }
                }
                CheckBox {
                    id: checkBoxRandomIdle
                    text: "Randomized idle motions"
                    checked: false
                }
                CheckBox {
                    id: checkBoxReact
                    text: "React to touch"
                    checked: true
                }
                RowLayout {
                    CheckBox {
                        id: checkBoxFollow
                        text: "Follow mouse"
                        checked: true
                    }
                    Button {
                        text: "Reset position"
                        onClicked: live2D.resetMouseFollowPosition()
                    }
                }
                CheckBox {
                    id: checkBoxBlink
                    text: "Blinking"
                    checked: true
                }
                CheckBox {
                    id: checkBoxBreath
                    text: "Breath"
                    checked: true
                }
                CheckBox {
                    id: checkBoxPhysics
                    text: "Physics"
                    checked: true
                }
                CheckBox {
                    id: checkBoxLip
                    text: "Lip sync test"
                    checked: false
                }
                Label {
                    text: "Log:"
                }
                ScrollView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    TextArea {
                        id: logText
                    }
                }
                /*
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
    Q_INVOKABLE void stopAllMotions();*/
            }
            Rectangle {
                Layout.fillWidth: true
            }
        }
    }

    Timer {
        id: lipSyncTimer
        running: true
        repeat: true
        interval: 16
        property double dt: 3.141526/60
        property double value: 0
        onTriggered: {
            value += dt
            live2D.lipSyncValue = Math.abs(Math.sin(value))
        }
    }

    width: 1600
    height: 1000
    visible: true
    title: qsTr("Live2DItem demo")
}
