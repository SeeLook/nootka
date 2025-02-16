/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ListView {
    id: buttBar

    orientation: ListView.Horizontal
    spacing: NOO.factor()
    width: parent.width
    height: NOO.factor() * 3
    currentIndex: 0
    leftMargin: NOO.isAndroid() ? 0 : NOO.factor() * 2
    clip: true

    Rectangle {
        z: -1
        width: buttBar.width
        height: buttBar.height
        color: NOO.isAndroid() ? activPal.base : Qt.darker(activPal.window, 1.1) // the same as DialogButtonBox
    }

    delegate: TabButton {
        id: tb

        onClicked: buttBar.currentIndex = index
        checked: buttBar.currentIndex === index
        anchors.verticalCenter: parent.verticalCenter
        Component.onCompleted: {
            background.color = Qt.binding(function() {
                return checked ? activPal.highlight : activPal.button;
            });
        }

        contentItem: Text {
            anchors.centerIn: parent
            font.pixelSize: NOO.factor() * 1.25
            text: (index + 1) + ". " + modelData
            color: checked ? activPal.highlightedText : activPal.buttonText
            style: tb.activeFocus ? Text.Sunken : Text.Normal
            styleColor: activPal.text
        }
        // only way to get access to TabButton components

    }

}
