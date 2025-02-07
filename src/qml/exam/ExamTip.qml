/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Window 2.12

GlowRect {
    id: tip

    property alias text: text.text
    property alias textItem: text
    property alias font: text.font
    property color bg: activPal.base
    property real offX: 0
    property real offY: 0
    property alias showExit: exitImg.visible
    // private
    property real eW: 0
    property real eH: 0

    color: Qt.tint(activPal.base, NOO.alpha(bg, 50))
    shadowRadius: NOO.factor() * 2
    radius: NOO.factor()
    cornerRadius: NOO.factor() * 2
    z: 200
    x: NOO.bound(NOO.factor(), offX - width / 2, executor.width - width - NOO.factor())
    y: NOO.bound(NOO.factor(), offY - height / 2, executor.height - height - NOO.factor())
    width: text.width
    height: text.height
    scale: GLOB.useAnimations ? 0 : 1
    Component.onCompleted: {
        eW = executor.width;
        eH = executor.height;
        if (GLOB.useAnimations)
            scale = 1;

    }

    border {
        width: NOO.factor() / 6
        color: bg
    }

    Text {
        id: text

        color: activPal.text
        textFormat: Text.RichText
        padding: NOO.factor()
        onLinkActivated: {
            executor.tipLink(link);
            tip.destroy();
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }

    }

    Image {
        id: exitImg

        visible: false
        source: NOO.pix("exit")
        sourceSize.width: tip.width / 20

        anchors {
            right: parent.right
            top: parent.top
            margins: tip.width / 80
        }

        MouseArea {
            anchors.fill: parent
            onClicked: tip.visible = false
        }

    }

    Connections {
        target: executor
        onWidthChanged: {
            scale = (executor.width / eW) * scale;
            offX = offX * (executor.width / eW);
            eW = executor.width;
        }
        onHeightChanged: {
            offY = offY * (executor.height / eH);
            eH = executor.height;
        }
    }

    Behavior on scale {
        SpringAnimation {
            spring: 2
            damping: 0.2
            epsilon: 0.005
            duration: 300
        }

    }

}
