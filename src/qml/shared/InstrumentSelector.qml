/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tumbler {
    id: instrTumb

    property alias instrument: instrTumb.currentIndex

    width: parent.width
    height: NOO.factor() * (NOO.isAndroid() ? 7 : 10)
    visibleItemCount: Math.min(((width / (height * 0.7)) / 2) * 2 - 1, 7)
    model: 9

    Rectangle {
        z: -1
        width: instrTumb.height * 0.9
        height: parent.height * 0.5
        x: parent.width / 2 - width / 2
        y: 2
        color: activPal.highlight
        radius: width / 12
    }

    delegate: Component {
        Item {
            opacity: 1 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: (1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)) * (textMa.pressed || glyphMa.pressed ? 0.8 : 1)

            Text {
                id: instrGlyph

                property color tc: NOO.randomColor()

                text: NOO.instr(modelData).glyph
                anchors.horizontalCenter: parent.horizontalCenter
                color: instrTumb.currentIndex === modelData ? activPal.highlightedText : tc

                font {
                    family: "nootka"
                    pixelSize: instrTumb.height * 0.3
                }

                MouseArea {
                    id: glyphMa

                    anchors.fill: parent
                    onClicked: instrTumb.currentIndex = modelData
                }

            }

            Text {
                width: instrTumb.height * 0.8
                text: NOO.instr(modelData).name.replace(" ", "\n")
                horizontalAlignment: Text.AlignHCenter
                color: activPal.text

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: instrGlyph.bottom
                }

                font {
                    bold: instrTumb.currentIndex === modelData
                    pixelSize: instrTumb.height * 0.08
                }

                MouseArea {
                    id: textMa

                    anchors.fill: parent
                    onClicked: instrTumb.currentIndex = modelData
                }

            }

        }

    }

    contentItem: PathView {
        id: pathView

        model: instrTumb.model
        delegate: instrTumb.delegate
        clip: true
        pathItemCount: instrTumb.visibleItemCount + 1
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        dragMargin: width / 2

        path: Path {
            startX: 0
            startY: instrTumb.height * 0.14

            PathLine {
                x: pathView.width
                y: instrTumb.height * 0.14
            }

        }

    }

}
