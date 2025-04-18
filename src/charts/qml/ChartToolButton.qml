/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

ToolButton {
    id: root

    property alias pixmap: pix.source
    property real factor: analyzeWindow.height / 150
    property Taction taction

    hoverEnabled: true
    width: pix.width + (NOO.isAndroid() ? 4 : factor * 2)
    height: pix.height + Screen.pixelDensity * 2
    enabled: (taction && taction.enabled) || !taction
    onClicked: {
        if (taction)
            taction.trigger();

        focus = false;
    }

    Image {
        id: pix

        mipmap: true
        anchors.centerIn: parent
        source: taction ? taction.icon : ""
        height: factor * 10
        width: height * (sourceSize.width / sourceSize.height)
        transformOrigin: Image.Center
        scale: !enabled || pressed ? 0.6 : (hovered ? 1.1 : 0.8)

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    ToolTip {
        id: toolTip

        delay: 750
        timeout: 5000
        visible: hovered && contentItem.text !== ""

        contentItem: Text {
            text: taction ? taction.text : ""
            color: activPal.highlightedText
        }

        enter: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "scale"
                to: 1
            }

        }

        exit: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "scale"
                to: 0
            }

        }

        background: TipRect {
            shadowRadius: NOO.factor()
            color: activPal.highlight
        }

    }

    background: Item {
    }

}
