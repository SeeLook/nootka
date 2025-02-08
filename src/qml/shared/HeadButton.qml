/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ToolButton {
    id: root

    property alias pixmap: pix.source
    property alias name: butText.text
    property real factor: nootkaWindow.width / 240
    property alias fontSize: butText.font.pixelSize
    property alias textColor: butText.color
    property Taction taction
    property bool hiHover: true

    padding: 0
    hoverEnabled: true
    width: Math.max(pix.width, butText.width) + (NOO.isAndroid() ? 4 : factor * 2)
    height: butText.height + pix.height + NOO.factor() / 4
    onHoveredChanged: {
        if (GLOB.showHints && taction && taction.tip !== "") {
            if (hovered)
                NOO.setStatusTip(taction.tip, taction.tipPos);
            else
                NOO.setStatusTip("", taction.tipPos);
        }
    }
    onClicked: {
        if (taction)
            taction.trigger();

        focus = false;
    }

    Text {
        id: butText

        text: taction ? taction.text : ""
        font.pixelSize: Math.min(factor * 2.5, NOO.factor())
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 1
        }
        horizontalAlignment: Text.AlignHCenter
        color: activPal.text
    }

    Image {
        id: pix

        mipmap: true
        source: taction ? taction.icon : ""
        y: NOO.factor() / 4 + (GLOB.useAnimations && !pressed && hiHover && hovered ? (root.height - height - NOO.factor() / 4) / 2 : 0)
        height: factor * 8
        width: height * (sourceSize.width / sourceSize.height)
        anchors.horizontalCenter: butText.horizontalCenter
        transformOrigin: Image.Center
        scale: pressed ? 0.9 : (GLOB.useAnimations && hiHover && hovered ? root.height / height : 1)

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

        Behavior on y {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    Connections {
        target: taction
        onShakeButton: shakeAnim.running = true
    }

    SequentialAnimation {
        id: shakeAnim

        running: false
        loops: 3

        NumberAnimation {
            target: root
            property: "rotation"
            from: 0
            to: -30
            duration: 50
        }

        NumberAnimation {
            target: root
            property: "rotation"
            from: -30
            to: 30
            duration: 100
        }

        NumberAnimation {
            target: root
            property: "rotation"
            from: 30
            to: 0
            duration: 50
        }

    }

    background: Item {
    }

}
