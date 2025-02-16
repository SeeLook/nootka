/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka
import Nootka.Charts
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

TlinChartDelegate {
    id: lineDel

    property var hintText: null

    onNrChanged: {
        if (groupNr > -1) {
            var h = getHintText();
            if (h !== "") {
                if (!hintText)
                    hintText = hintComp.createObject(lineDel);

                hintText.text = h;
            } else {
                if (hintText)
                    hintText.destroy();

            }
        }
    }

    // average line
    Rectangle {
        visible: averageY > 0
        color: chartItem.averLineGroup === groupNr ? "#00c0c0" : (groupNr % 2 ? "#008080" : "#00a0a0")
        x: width / 2
        z: 1
        y: parent.height * 0.1 + maxDataHeight - (averageY / chartItem.maxYValue()) * maxDataHeight - height / 2
        width: parent.width
        height: lThick / 2

        MouseArea {
            id: averMa

            width: parent.width
            height: parent.height * 5
            y: -2 * parent.height
            hoverEnabled: true
            onEntered: lineEntered(mouseX, parent.y)
            onExited: lineExited()
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

                // question number
                Text {
                    text: nrText
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: activPal.text
                    textFormat: Text.StyledText
                    horizontalAlignment: Text.AlignHCenter

                    font {
                        pixelSize: lineDel.width / 7
                    }

                }

                // note name
                Item {
                    width: childrenRect.width
                    height: lineDel.width / 4
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        y: -lineDel.width / (nrText === "" ? 5 : 4.5)
                        text: noteText
                        color: activPal.text
                        textFormat: Text.StyledText

                        font {
                            pixelSize: lineDel.width / 5
                            family: "Scorek"
                        }

                    }

                }

                // position on instrument
                Text {
                    text: posText
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: activPal.text
                    textFormat: Text.RichText

                    font {
                        pixelSize: lineDel.width / 6
                    }

                }

                // key signature
                Text {
                    text: keyText
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: activPal.text
                    textFormat: Text.PlainText

                    font {
                        pixelSize: lineDel.width / 7
                        italic: true
                    }

                }

            }

        }

    }

    Text {
        id: nText

        color: pointColor
        text: pointSymbol
        y: pointY - (pointSymbol === "M" ? height * 0.3 : height * 0.8)
        x: parent.width - (pointSymbol === "M" ? width * 0.5 : width / 3)
        visible: false

        font {
            pixelSize: lineDel.width / 2
            family: "Nootka"
        }

    }

    DropShadow {
        z: 5010
        anchors.fill: nText
        horizontalOffset: lineDel.width / 35
        verticalOffset: horizontalOffset
        radius: NOO.factor()
        samples: 1 + radius * 2
        color: activPal.shadow
        source: nText
        scale: ma.containsMouse ? 1.4 : 1

        MouseArea {
            id: ma

            anchors.fill: parent
            hoverEnabled: true
            onEntered: pointEntered()
            onExited: pointExited()
        }

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    Component {
        id: hintComp

        Text {
            z: -1
            x: (parent.width - width) / 2
            y: parent.height * 0.05
            color: NOO.alpha(activPal.text, 75)
            textFormat: Text.RichText
        }

    }

}
