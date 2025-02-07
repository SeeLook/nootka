/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import ".."
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Popup {
    property alias enableMetronome: metroVisibleChB.checked
    property alias count: countChB.checked
    // private
    property var beatFactor: [1, 2, 0.666667, 0.5]
    readonly property string quantTip: qsTr("Detected rhythmic units are rounded (quantization). Shortest units require more rhythmical accuracy.")
    property real lastTime: new Date().getTime()

    signal accepted()

    function tapTempo() {
        var currTime = new Date().getTime();
        if (currTime - lastTime < 1500)
            tempoSpin.value = Math.round(60000 / (currTime - lastTime));

        lastTime = currTime;
    }

    margins: NOO.factor()
    scale: GLOB.useAnimations ? 0 : 1
    x: NOO.isAndroid() ? (nootkaWindow.width - width) / 2 : 0
    y: NOO.isAndroid() ? (nootkaWindow.height - height) / 2 : 0
    onAboutToShow: {
        SOUND.stop();
        spaceShort.enabled = true;
        tempoSpin.value = SOUND.tempo;
        buTumb.currentIndex = SOUND.beatUnit;
    }
    onAboutToHide: {
        SOUND.startListen();
        spaceShort.enabled = false;
        SOUND.setMetronome(tempoSpin.value, buTumb.currentIndex);
    }

    Column {
        // Row
        //visible: NOO.isAndroid()
        //width: parent.width
        //height: NOO.factor() * 2.8
        //anchors.horizontalCenter: parent.horizontalCenter
        //color: ma2.containsPress ? NOO.alpha(activPal.highlight, 50) : "transparent"
        //Text {
        //anchors.verticalCenter: parent.verticalCenter
        //text: "\u018f"; font { family: "Nootka"; pixelSize: NOO.factor() * 2.5 }
        //color: SOUND.tickDuringPlay ? "#00a0a0" : activPal.text
        //}
        //Text {
        //anchors.verticalCenter: parent.verticalCenter
        //x: NOO.factor() * 4; font.pixelSize: NOO.factor()
        //text: NOO.TR("TempoBar", "Audible metronome")
        //}
        //MouseArea {
        //id: ma2
        //anchors.fill: parent
        //onClicked: SOUND.tickDuringPlay = !SOUND.tickDuringPlay
        //onPressAndHold: NOO.setStatusTip(NOO.TR("TempoBar", "Audible metronome") + ".<br>" + NOO.TR("TempoBar", "Use earphones! Otherwise ticking will disturb proper pitch detection!"))
        //onReleased: NOO.setStatusTip("")
        //}
        //Rectangle { color: activPal.text; width: parent.width; height: 1; anchors.bottom: parent.bottom }
        //}

        spacing: NOO.factor() / 2

        Row {
            spacing: NOO.factor()

            Tumbler {
                id: buTumb

                property int prevIndex: -1

                anchors.verticalCenter: parent.verticalCenter
                height: NOO.factor() * 6
                width: NOO.factor() * 2
                model: ["\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3"]
                visibleItemCount: 3
                wrap: true
                currentIndex: SOUND.beatUnit
                onCurrentIndexChanged: {
                    if (prevIndex > -1)
                        tempoSpin.value = Math.round(tempoSpin.value * (beatFactor[currentIndex] / beatFactor[prevIndex]));

                    prevIndex = currentIndex;
                }

                MouseArea {
                    anchors.fill: parent
                    z: -1
                    onWheel: {
                        var ci = buTumb.currentIndex;
                        if (wheel.angleDelta.y > 0)
                            ci--;
                        else if (wheel.angleDelta.y < 0)
                            ci++;
                        if (ci >= buTumb.count)
                            ci = 0;
                        else if (ci < 0)
                            ci = buTumb.count - 1;
                        buTumb.currentIndex = ci;
                    }
                }
                // Tumbler

                background: TipRect {
                    color: buTumb.enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
                    radius: 0
                }

                delegate: Rectangle {
                    color: index === buTumb.currentIndex ? activPal.highlight : activPal.base
                    opacity: 1 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)

                    Text {
                        text: modelData
                        y: height * 0.325
                        height: parent.height
                        width: parent.width
                        color: index === buTumb.currentIndex ? activPal.highlightedText : activPal.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        font {
                            pixelSize: parent.height * 0.75
                            family: "Scorek"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: buTumb.currentIndex = index
                        }

                    }

                }

            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "="
                color: activPal.text

                font {
                    pixelSize: NOO.factor() * 2
                }

            }

            Column {
                anchors.verticalCenter: parent.verticalCenter

                Row {
                    spacing: NOO.factor()
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        text: qsTr("tempo")
                        font.pixelSize: NOO.factor()
                        color: activPal.text
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TspinBox {
                        id: tempoSpin

                        focus: false
                        from: 40
                        to: Math.min(240, 180 * beatFactor[buTumb.currentIndex])
                        editable: true
                        value: SOUND.tempo
                    }

                }

                Tslider {
                    focus: false
                    width: parent.width * 0.96
                    anchors.horizontalCenter: parent.horizontalCenter
                    value: tempoSpin.value
                    from: 40
                    to: tempoSpin.to
                    onValueChanged: tempoSpin.value = value
                    stepSize: 10
                }

            }

        }

        TiconButton {
            width: parent.width - NOO.factor()
            text: qsTr("Tap tempo")
            pixmap: NOO.pix("fingerpoint")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: tapTempo()
            focus: true
        }

        TcheckBox {
            id: metroVisibleChB

            visible: !NOO.isAndroid()
            text: qsTr("Metronome visible")
            checked: true
        }

        TcheckBox {
            id: countChB

            visible: !NOO.isAndroid()
            x: NOO.factor()
            enabled: metroVisibleChB.checked
            text: qsTr("Count up")
            checked: true
        }

        Rectangle {
            visible: NOO.isAndroid()
            width: parent.width
            height: NOO.factor() * 2.8
            anchors.horizontalCenter: parent.horizontalCenter
            color: ma.containsPress ? NOO.alpha(activPal.highlight, 50) : "transparent"

            Rectangle {
                color: activPal.text
                width: parent.width
                height: 1
                y: -1
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "\u0190"
                color: SOUND.tickBeforePlay ? "#00a0a0" : activPal.text

                font {
                    family: "Nootka"
                    pixelSize: NOO.factor() * 2.5
                }

            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                x: NOO.factor() * 4
                font.pixelSize: NOO.factor()
                text: qsTr("Count up")
            }

            MouseArea {
                id: ma

                anchors.fill: parent
                onClicked: SOUND.tickBeforePlay = !SOUND.tickBeforePlay
                onPressAndHold: NOO.setStatusTip(NOO.TR("TempoBar", "Countdown before playing or listening."))
                onReleased: NOO.setStatusTip("")
            }

            Rectangle {
                color: activPal.text
                width: parent.width
                height: 1
                y: parent.height + NOO.factor() / 4 - 1
            }

        }
        //Rectangle { TODO: Not yet implemented under Android

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: radioRow.height
            enabled: !executor

            ButtonGroup {
                buttons: radioRow.children
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: !NOO.isAndroid()
                onEntered: NOO.setStatusTip(quantTip, Item.TopLeft)
                onExited: NOO.setStatusTip("", Item.TopLeft)
                onPressAndHold: {
                    if (NOO.isAndroid())
                        NOO.setStatusTip(quantTip);

                }
                onReleased: {
                    if (NOO.isAndroid())
                        NOO.setStatusTip("");

                }
            }

            Row {
                id: radioRow

                spacing: NOO.factor()
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: qsTr("round to:")
                    font.pixelSize: NOO.factor()
                    color: activPal.text
                    anchors.verticalCenter: parent.verticalCenter
                }

                TradioButton {
                    id: radio16

                    text: "G"
                    textScale: 2
                    checked: SOUND.quantization === 6
                    onClicked: SOUND.quantization = 6
                    onHoveredChanged: NOO.setStatusTip(hovered ? quantTip : "", Item.TopLeft)

                    font {
                        family: "Nootka"
                    }

                }

                TradioButton {
                    id: radio8

                    text: "F"
                    textScale: 2
                    checked: SOUND.quantization === 12
                    onClicked: SOUND.quantization = 12
                    onHoveredChanged: NOO.setStatusTip(hovered ? quantTip : "", Item.TopLeft)

                    font {
                        family: "Nootka"
                    }

                }

            }

        }

    }

    Shortcut {
        id: spaceShort

        sequence: " "
        onActivated: tapTempo()
    }

    background: GlowRect {
        color: activPal.window
        shadowRadius: NOO.factor() / 2
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

}
