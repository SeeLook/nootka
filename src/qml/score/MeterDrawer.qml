/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import Nootka.Music

// meter menu
Drawer {
    // private
    property var colorArr: [0, 1, 1, 0]

    visible: true
    width: nootkaWindow.width / 5
    height: nootkaWindow.height

    GridView {
        id: meterGrid

        anchors.fill: parent
        clip: true
        contentHeight: childrenRect.height
        cellHeight: NOO.factor() * 5
        cellWidth: parent.width / 2
        model: 12

        delegate: Rectangle {
            height: NOO.factor() * 5 - 2
            width: parent.width / 2 - 2
            color: score.scoreObj.meter === Math.pow(2, index) ? activPal.highlight : (area.containsMouse ? Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50)) : (colorArr[index % 4] === 1 ? activPal.alternateBase : activPal.base))

            Text {
                id: buttText

                anchors.horizontalCenter: parent.horizontalCenter
                y: -NOO.factor() * 4.5
                text: NOO.meter(Math.pow(2, index)).symbol()
                color: score.scoreObj.meter === Math.pow(2, index) ? activPal.highlightedText : activPal.text

                font {
                    family: "Scorek"
                    pixelSize: NOO.factor() * 4
                }

            }

            MouseArea {
                id: area

                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    score.scoreObj.setMeter(Math.pow(2, index));
                    meter.text = buttText.text;
                    close();
                }
            }

        }

    }

    background: GlowRect {
        color: activPal.window
        cornerRadius: NOO.factor() / 2
        radius: 0
    }

}
