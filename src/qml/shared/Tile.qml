/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12

/**
 * 12 is magical number of pixels that corresponds with tipbg.png image size and keeps the frame pretty at any circumstances
 */
BorderImage {
    property alias description: descText.text
    property alias descriptionColor: descText.color
    property alias bgColor: bg.color
    property alias bgBorder: bg.border
    default property alias content: container.data

    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    width: parent.width
    height: (descText.text === "" ? 0 : descText.height + NOO.factor()) + container.height + NOO.factor() * (NOO.isAndroid() ? 1.5 : 2.5)
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
    source: NOO.pix("tipbg")

    // background
    Rectangle {
        id: bg

        x: 6
        y: 6
        width: parent.width - 12
        height: parent.height - 12
        color: enabled ? Qt.lighter(activPal.window, 1.05) : Qt.darker(disdPal.window, 1.5)
        radius: 6

        gradient: Gradient {
            GradientStop {
                position: 0.5
                color: enabled ? Qt.lighter(bg.color, 1.05) : Qt.darker(disdPal.window, 1.5)
            }

            GradientStop {
                position: 0.8
                color: enabled ? Qt.lighter(bg.color, 0.95) : Qt.darker(disdPal.window, 1.75)
            }

        }

    }

    border {
        left: 12
        right: 12
        bottom: 12
        top: 12
    }

    Column {
        spacing: NOO.factor() / 2
        width: parent.width

        // spacer
        Item {
            width: parent.width
            height: NOO.factor() * (NOO.isAndroid() ? 0.25 : 0.5)
        }

        Item {
            id: container

            width: parent.width - NOO.factor()
            height: childrenRect.height
        }

        Text {
            id: descText

            anchors.bottom: parent.Bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.96
            font.pixelSize: NOO.factor() * 0.8
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            color: enabled ? activPal.text : disdPal.text
            wrapMode: Text.WordWrap
        }

    }

}
