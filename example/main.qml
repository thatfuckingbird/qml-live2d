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
import Live2D 1.0

Window {
    flags: Qt.WA_TranslucentBackground
    color: "transparent"
    Live2DItem {
        anchors.left: parent.left
        width: 1000
        height: 1000
        modelPath: "data/Hiyori"
        playRandomMotions: true
    }

    Live2DItem {
        id: live2D2
        backgroundColor: "gray"
        anchors.right: parent.right
        width: 1000
        height: 1000
        modelPath: "data/Hiyori"
    }

    width: 2000
    height: 1000
    visible: true
    title: qsTr("Live2DItem demo")
}
