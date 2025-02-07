/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import QtQuick.Controls 2.12
import Score 1.0

ListView {
    id: clefMenu

    property int selClef
    // private
    property var clefNr: [0, 1, 2, 4, 8, 32, 128]

    signal clicked(var cl)

    function checkClef() {
        for (var c = 0; c < 7; ++c) {
            // so far seven clefs are supported
            if (clefNr[c] === selClef)
                clefMenu.currentIndex = c;

        }
    }

    width: parent.width
    height: parent.height
    clip: true
    spacing: 1
    onSelClefChanged: checkClef()
    onVisibleChanged: {
        if (visible)
            checkClef();

    }
    model: 7

    ScrollBar.vertical: ScrollBar {
        active: false
        visible: active
    }

    footer: Item {
        width: parent.width
        height: NOO.factor() * 2
    }

    delegate: Rectangle {
        width: clefMenu.width
        height: visible ? NOO.factor() * (index === 6 ? 10 : 7.5) : 0
        visible: index !== 0 || score.meter !== Tmeter.NoMeter
        color: index === clefMenu.currentIndex ? activPal.highlight : (area.containsMouse ? Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50)) : (index % 2 === 1 ? activPal.alternateBase : activPal.base))

        Row {
            height: parent.height
            width: parent.width
            spacing: NOO.factor()

            Item {
                width: NOO.factor() * 3.2
                height: NOO.factor()

                Text {
                    scale: NOO.factor() * 0.9
                    x: scale * 2.5
                    y: 4.6 * NOO.factor()
                    text: NOO.clef(clefNr[index]).glyphOnStaff()
                    color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text

                    font {
                        family: "scorek"
                        pixelSize: 3
                    }

                }

            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - NOO.factor() * 4.8
                spacing: NOO.factor() / 4

                Text {
                    text: NOO.clef(clefNr[index]).name()
                    color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text

                    font {
                        bold: true
                        pixelSize: NOO.factor() * 1.1
                    }

                }

                Text {
                    text: NOO.clef(clefNr[index]).desc()
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text

                    font {
                        pixelSize: NOO.factor() * 0.8
                    }

                }

            }

        }

        MouseArea {
            id: area

            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                selClef = clefNr[index];
                clefMenu.currentIndex = index;
                clefMenu.clicked(clefNr[index]);
            }
        }

    }

}
