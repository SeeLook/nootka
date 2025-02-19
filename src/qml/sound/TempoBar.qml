/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12

Item {
    id: tempoBar

    // private
    property var tMenu: null
    property int hiTick: -1
    property int cnt: 1
    property var hArray: [0.6, 0, 0.3, 0, 0.6]
    property var gArray: ["\ue1d5", "\ue1d9", "\ue1d7", "\ue1d9", "\ue1d5"]
    property int countTo: NOO.meter(score.meter).countTo()
    property var preCountItem: null
    // protected
    property var beatModel: ["\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3"]
    property real contW: metroText.width + cntBeforeBut.width + metroRow.width + loudTickButt.width

    Row {
        width: parent.width
        height: parent.height
        spacing: (width - contW) / 4 - 1

        RectButton {
            id: metroText

            forcedHeight: parent.height
            yOffset: parent.height * -0.7
            statusTip: qsTr("Tempo") + " - " + qsTr("beats per minute") + "<br>" + qsTr("Mouse wheel changes tempo promptly (also with %1 key). Click for more options.").arg(NOO.isMac() ? "CMD" : "CTRL")
            text: beatModel[SOUND.beatUnit] + "=" + SOUND.tempo

            font {
                family: "Scorek"
                pixelSize: parent.height * 0.7
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!tMenu) {
                        var c = Qt.createComponent("qrc:/sound/TempoMenu.qml");
                        tMenu = c.createObject(tempoBar);
                    }
                    tMenu.open();
                }
                onWheel: wheel => {
                    if (!SOUND.metroRunning) {
                        if (wheel.angleDelta.y > 0)
                            SOUND.tempo = SOUND.tempo + (wheel.modifiers & Qt.ControlModifier ? 10 : 1);
                        else if (wheel.angleDelta.y < 0)
                            SOUND.tempo = SOUND.tempo - (wheel.modifiers & Qt.ControlModifier ? 10 : 1);
                    }
                }
            }

        }

        RectButton {
            id: cntBeforeBut

            enabled: !SOUND.metroRunning
            text: "\u0190"
            statusTip: qsTr("Countdown before playing or listening.")
            checked: SOUND.tickBeforePlay
            textColor: checked ? "#00a0a0" : activPal.text
            onClicked: SOUND.tickBeforePlay = !SOUND.tickBeforePlay

            font {
                family: "Nootka"
                pixelSize: parent.height
            }

        }

        Row {
            id: metroRow

            enabled: !tMenu || tMenu.enableMetronome
            height: tempoBar.height
            spacing: height / (SOUND.tempo < 110 ? 5 : 2)

            Repeater {
                id: rep

                model: 5

                Rectangle {
                    readonly property color bgColor: Qt.tint(activPal.window, NOO.alpha(activPal.base, 100))

                    y: parent.height * (Math.abs(0.6 - hArray[index]) / 5)
                    width: parent.height * (0.9 - (Math.abs(0.6 - hArray[index]) / 3))
                    height: parent.height * 0.8 + hArray[index] * parent.height
                    radius: width / 2
                    color: index === hiTick && timer.running && enabled ? (index === 0 || index === 4 ? "#00a0a0" : activPal.text) : bgColor
                    rotation: (index - 2) * 30
                    visible: SOUND.tempo < 110 || index % 2 !== 1

                    // rhythm
                    Text {
                        visible: !enabled || (index !== hiTick || !SOUND.metroRunning)
                        color: enabled ? activPal.text : disdPal.text
                        text: gArray[index]
                        y: parent.height * -0.85
                        x: (parent.width - width) / 2

                        font {
                            pixelSize: parent.height * 0.8
                            family: "Scorek"
                        }

                    }

                    // count
                    Text {
                        visible: timer.running && index === hiTick && (!tMenu || (tMenu.count && tMenu.enableMetronome))
                        color: activPal.base
                        text: cnt
                        y: parent.height * -1.2
                        x: (parent.width - width) / 2

                        font {
                            pixelSize: parent.height
                            family: "Scorek"
                        }

                    }

                }

            }

        }

        RectButton {
            id: loudTickButt

            enabled: !SOUND.metroRunning
            text: "\u018f"
            statusTip: qsTr("Audible metronome") + ".<br>" + qsTr("Use earphones! Otherwise ticking will disturb proper pitch detection!")
            checked: SOUND.tickDuringPlay
            textColor: checked ? "#00a0a0" : activPal.text
            onClicked: SOUND.tickDuringPlay = !SOUND.tickDuringPlay

            font {
                family: "Nootka"
                pixelSize: parent.height
            }

        }

        Timer {
            id: timer

            property real elap: 0
            property real lag: 0
            property int phase: 0

            running: tempoBar.visible && SOUND.metroRunning
            repeat: true
            interval: (SOUND.tempo < 110 ? 15000 : 30000) / SOUND.tempo
            onRunningChanged: {
                if (running) {
                    elap = 0;
                    lag = 0;
                    phase = 0;
                    hiTick = 4;
                } else {
                    cnt = 1; // reset it here - countdown number depends on it when running
                }
            }
            onTriggered: {
                var currTime = new Date().getTime();
                if (elap > 0) {
                    elap = currTime - elap;
                    lag += elap - interval;
                }
                elap = currTime;
                interval = Math.max(((SOUND.tempo < 110 ? 15000 : 30000) / SOUND.tempo) - lag, 1);
                lag = 0;
                if ((phase + (SOUND.tempo < 110 ? 1 : 2)) % 4 === 0) {
                    if (cnt < countTo)
                        cnt++;
                    else
                        cnt = 1;
                }
                phase += SOUND.tempo < 110 ? 1 : 2;
                if (phase > 7)
                    phase = 0;

                hiTick = Math.abs(phase - 4);
            }
        }

    }

    MouseArea {
        x: metroRow.x
        y: metroRow.y
        width: metroRow.width
        height: metroRow.height
        hoverEnabled: true
        onEntered: NOO.setStatusTip(qsTr("Metronome"), Item.TopLeft)
        onExited: NOO.setStatusTip("", Item.TopLeft)
    }

    Connections {
        target: SOUND
        function onCountdownPrepare(tickCount) : void {
            if (SOUND.tickBeforePlay && !GLOB.singleNoteMode) {
                if (!preCountItem) {
                    preCountItem = Qt.createComponent("qrc:/sound/CountdownItem.qml").createObject(score);
                }
                preCountItem.tickCount = tickCount;
                preCountItem.counter = 1;
            }
        }
    }

}
