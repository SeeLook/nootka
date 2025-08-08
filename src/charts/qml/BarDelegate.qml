/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Charts 1.0
import QtQuick.Effects
import QtQuick 2.12

Item {
    id: barDel

    property alias chart: barIt.chart
    property alias groupNr: barIt.groupNr

    TbarChartDelegate {
        id: barIt

        x: parent.width * 0.75
        y: parent.height * 0.1 + maxDataHeight - (yValue / chartItem.maxYValue()) * maxDataHeight
        width: parent.width / 2
        height: maxDataHeight * 1.14 - y
        visible: false
        hovered: barMA.containsMouse
    }

    MultiEffect {
        z: 5010
        source: barIt
        anchors.fill: barIt
        shadowEnabled: true
        shadowHorizontalOffset: barDel.width / 30
        shadowVerticalOffset: barDel.width / 30
        shadowColor: barIt.hovered ? activPal.highlight : activPal.shadow
        scale: barMA.containsMouse ? 1.1 : 1

        MouseArea {
            id: barMA

            anchors.fill: parent
            hoverEnabled: true
        }

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    // X axis line (partial)
    Rectangle {
        color: activPal.text
        y: parent.height - width + lThick / 2
        width: parent.width
        height: lThick

        // X tick
        Rectangle {
            color: activPal.text
            x: parent.width - lThick
            y: lThick
            width: lThick
            height: lThick * 2

            // X value (description)
            Column {
                y: lThick * 2
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: barIt.xText
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: activPal.text
                    textFormat: Text.RichText
                    horizontalAlignment: Text.AlignHCenter

                    font {
                        pixelSize: barDel.width / 5
                    }

                }

            }

        }

    }

}
