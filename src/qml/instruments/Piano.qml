/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12

TpianoBg {
    id: instrItem

    property alias keysX: keysRow.x
    property alias keysWidth: keysRow.width
    //   private
    property var activeKey: null
    property var correctAnim: null
    property var pianoZoom: null

    function getKey(keyNr) {
        return whiteRep.itemAt(keyNr);
    }

    width: Math.max(nootkaWindow.width, GLOB.instrument.getItemHeight(nootkaWindow.shortEdge) * 6.7)
    height: GLOB.instrument.getItemHeight(nootkaWindow.shortEdge)
    onWantKeyToSelect: (k, isWhite) => {
        selectKey(k > -1 ? (isWhite ? whiteRep.itemAt(k) : whiteRep.itemAt(k).black) : null);
    }
    transformOrigin: Item.TopLeft
    onCorrectInstrument: {
        if (!correctAnim)
            correctAnim = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml").createObject(instrItem);

        correctAnim.doCross = !wrongItem.parent;
        correctAnim.start();
    }
    Component.onCompleted: {
        if (NOO.isAndroid() && NOO.fingerPixels() * 4 > height)
            pianoZoom = Qt.createComponent("qrc:/instruments/InstrumentZoom.qml").createObject(instrItem);

    }
    onWantNoteName: {
        if (!extraName) {
            extraName = Qt.createComponent("qrc:/instruments/ExtraName.qml").createObject(instrItem);
            extraName.fSize = Qt.binding(function() {
                return keyWidth * 2;
            });
        }
        if (origin) {
            extraName.text = name;
            extraName.x = Qt.binding(function() {
                return keysRow.x + (origin ? origin.nr * keyWidth + (origin.width - extraName.width) / 2 : 0);
            });
            extraName.y = Qt.binding(function() {
                return instrItem.height / 2 - extraName.height * (origin && origin.black ? 0.52 : 0.3);
            });
        } else {
            extraName.text = "";
        }
    }

    // piano background
    Image {
        parent: nootkaWindow.contentItem
        cache: false
        source: NOO.pix("pianoBg")
        width: instrItem.width
        height: width * (sourceSize.height / sourceSize.width)
        x: -instrFlick.contentX
        y: parent.height - instrItem.height * instrItem.scale - height
        transformOrigin: Item.BottomLeft
        scale: instrItem.scale
    }

    // black background
    Rectangle {
        width: parent.width
        height: parent.height
        color: "black"

        // keys
        Row {
            id: keysRow

            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                id: whiteRep

                model: keysNumber

                PianoKeyWhite {
                    nr: index
                    onEntered: key => { activeKey = key; }
                    onClicked: key => { selectedKey = key; }
                }

            }

        }

    }

    // piano key cursor
    Rectangle {
        anchors.fill: parent ? parent : undefined
        parent: activeKey
        z: 2
        color: GLOB.fingerColor
        radius: width / 5
        visible: active

        border {
            width: Math.round(keyWidth / 16)
            color: "black"
        }

    }

    Rectangle {
        id: octaveCover

        width: parent.width
        height: NOO.factor() * (NOO.isAndroid() ? 1 : 1.5)
        color: "black"

        Repeater {
            model: Math.floor(keysNumber / 7)

            Rectangle {
                x: keysRow.x + index * width
                width: keyWidth * 7
                height: parent.height
                color: index % 2 ? "#303030" : "black"

                Text {
                    anchors.centerIn: parent
                    text: octaveName(firstOctave + index) + (GLOB.scientificOctaves ? "  [%1]".arg(firstOctave + index + 3) : "")
                    color: "white"

                    font {
                        pixelSize: parent.height * 0.8
                    }

                }

            }

        }

    }

    Behavior on scale {
        enabled: GLOB.useAnimations

        NumberAnimation {
            duration: 150
            onRunningChanged: {
                if (pianoZoom && !running && scale > 1)
                    instrFlick.contentX = pianoZoom.flickX;

            }
        }

    }

    // piano key highlight
    keyHighlight: Rectangle {
        parent: selectedKey
        anchors.fill: parent ? parent : undefined
        color: GLOB.selectedColor
        radius: width / 5
        z: 2

        border {
            width: Math.round(keyWidth / 16)
            color: "black"
        }

    }

}
