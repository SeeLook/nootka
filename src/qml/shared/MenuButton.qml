/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: menuButton

    property Taction action
    property alias text: butText.text
    property alias iconHeight: icon.height
    property alias containsMouse: ma.containsMouse
    property alias containsPress: ma.containsPress
    property alias textColor: butText.color
    // private
    property var shortText: null

    signal clicked()

    function buttonClicked() {
        menuButton.clicked();
        if (action)
            action.trigger();

    }

    width: parent.width - NOO.factor()
    implicitHeight: NOO.factor() * 2.8
    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    color: ma.containsPress ? activPal.highlight : (ma.containsMouse ? NOO.alpha(activPal.highlight, 50) : "transparent")
    enabled: !action || action.enabled
    scale: GLOB.useAnimations && ma.pressed ? 0.9 : 1
    onActionChanged: {
        if (action && action.checkable)
            radioComp.createObject(menuButton);

    }
    Component.onCompleted: {
        // shortcut is known only now
        if (!NOO.isAndroid() && action && action.shortcut)
            shortText = shortComp.createObject(menuButton);

    }

    Image {
        id: icon

        source: action && !action.checkable ? action.icon : ""
        x: NOO.factor() / 2
        height: NOO.factor() * 2.2
        width: height * (sourceSize.width / sourceSize.height)
        anchors.verticalCenter: parent.verticalCenter
        scale: GLOB.useAnimations && !ma.pressed && ma.containsMouse ? 1.2 : 1
        visible: menuButton.enabled

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    Text {
        id: butText

        x: NOO.factor() * (action && (action.icon !== "" || action.checkable) ? 3.7 : 0.8)
        text: action ? action.text : ""
        color: enabled ? activPal.text : disdPal.text
        width: parent.width - x - NOO.factor() - (shortText ? shortText.width : 0)
        height: parent.height
        verticalAlignment: Text.AlignVCenter
        fontSizeMode: Text.HorizontalFit
        minimumPixelSize: NOO.factor() / 2
        minimumPointSize: minimumPixelSize
        elide: Text.ElideRight

        font {
            bold: true
            pixelSize: NOO.factor()
        }

    }

    Component {
        id: radioComp

        TcheckBox {
            anchors.verticalCenter: parent.verticalCenter
            checked: menuButton.action.checked
            onClicked: buttonClicked()
            x: (NOO.factor() * 3.5 - width) / 2
        }

    }

    Component {
        id: shortComp

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: action.key()
            font.pixelSize: NOO.factor() * 0.8
            x: menuButton.width - width - NOO.factor() / 2
            color: enabled ? activPal.text : disdPal.text
        }

    }

    MouseArea {
        id: ma

        anchors.fill: parent
        hoverEnabled: true
        onClicked: buttonClicked()
        onHoveredChanged: {
            if (action) {
                if (GLOB.showHints && action.tip !== "" && ma.containsMouse)
                    NOO.setStatusTip(action.tip, action.tipPos);
                else
                    NOO.setStatusTip("", action.tipPos);
            }
        }
    }

    Behavior on scale {
        enabled: GLOB.useAnimations

        NumberAnimation {
            duration: 150
        }

    }

}
