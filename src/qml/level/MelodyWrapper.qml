/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../score"
import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import Score 1.0

MouseArea {
    id: wrapArea

    property bool held: false
    property alias nr: wrapper.nr
    property alias outOfScale: wrapper.outOfScale
    property alias wrapper: wrapper

    function updateMelody() {
        wrapper.updateMelody();
    }

    function reload() {
        wrapper.reload();
    }

    height: NOO.factor() * 10
    hoverEnabled: false // triggered with delay to avoid highlighting all wrappers
    drag.target: wrapArea.held ? wrapper : undefined
    drag.axis: Drag.YAxis
    onPressAndHold: wrapArea.held = true
    onReleased: wrapArea.held = false
    onClicked: melListView.currentMelody = nr
    cursorShape: held ? Qt.SizeVerCursor : (containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor)

    TmelodyWrapper {
        id: wrapper

        score: sc.scoreObj
        melodyView: melListView
        width: wrapArea.width
        height: wrapArea.height
        Drag.active: wrapArea.held
        Drag.source: wrapArea
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Score {
            id: sc

            property color base: wrapper.outOfScale ? "red" : activPal.base

            anchors.fill: parent
            interactive: false
            readOnly: true
            bgColor: Qt.tint(activPal.base, NOO.alpha(wrapArea.held ? activPal.text : (nr === melListView.currentMelody ? activPal.highlight : base), 50))

            Behavior on bgColor {
                enabled: GLOB.useAnimations

                ColorAnimation {
                    duration: 150
                }

            }

        }

        // this is part of covering rectangle
        Rectangle {
            height: parent.height
            width: parent.height
            rotation: -90
            x: parent.width - scoreCover.width - width

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "transparent"
                }

                GradientStop {
                    position: 0.75
                    color: sc.bgColor
                }

            }

        }

        Rectangle {
            id: scoreCover

            height: parent.height
            width: parent.width - parent.height * 4
            color: sc.bgColor

            anchors {
                right: parent.right
            }

            Text {
                visible: wrapper.outOfScale
                font.pixelSize: parent.height * 0.2
                text: NOO.TR("OutScaleTip", "Out of instrument scale!")
                color: "red"

                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 10
                }

            }

            Row {
                y: parent.height / 3
                spacing: NOO.factor() / 2

                anchors {
                    right: parent.right
                    margins: 10
                }

                RectButton {
                    height: scoreCover.height / 4
                    text: "\u2702"
                    textColor: wrapArea.containsMouse || NOO.isAndroid() ? activPal.text : NOO.alpha(activPal.text, 30)
                    onClicked: {
                        melListView.currWrapp = wrapArea;
                        melListView.showDividePopup();
                    }

                    font {
                        family: "Nootka"
                        pixelSize: scoreCover.height / 4
                    }

                    Behavior on textColor {
                        enabled: GLOB.useAnimations

                        ColorAnimation {
                        }

                    }

                }

                RectButton {
                    height: scoreCover.height / 4
                    text: "\u0192"
                    textColor: wrapArea.containsMouse || NOO.isAndroid() ? activPal.text : NOO.alpha(activPal.text, 30)
                    onClicked: {
                        melListView.currWrapp = wrapArea;
                        melListView.showTransposePopup();
                    }

                    font {
                        family: "Nootka"
                        pixelSize: scoreCover.height / 4
                    }

                    Behavior on textColor {
                        enabled: GLOB.useAnimations

                        ColorAnimation {
                        }

                    }

                }

            }

            RectButton {
                visible: !NOO.isAndroid()
                height: parent.height / 3
                text: "\u0191"
                textColor: wrapArea.containsMouse || NOO.isAndroid() ? activPal.text : NOO.alpha(activPal.text, 30)
                onClicked: melListView.showMelody(wrapArea)

                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    margins: 10
                }

                font {
                    family: "Nootka"
                    pixelSize: parent.height / 3
                }

                Behavior on textColor {
                    enabled: GLOB.useAnimations

                    ColorAnimation {
                    }

                }

            }

        }

        Text {
            x: NOO.factor() / 2
            y: NOO.factor() / 2
            text: nr + 1
            color: NOO.alpha(activPal.text, 150)

            font {
                bold: true
            }

        }

        Text {
            text: wrapper.title
            x: (parent.width - width) / 2
            y: NOO.factor() / 4
            minimumPixelSize: NOO.factor() * 0.9
            fontSizeMode: Text.HorizontalFit
            color: wrapArea.containsMouse ? activPal.text : NOO.alpha(activPal.text, 150)
            width: Math.min(NOO.factor() * 25, parent.width - NOO.factor() * 6)
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter

            font {
                bold: true
                pixelSize: NOO.factor() * 1.3
            }

            Behavior on color {
                enabled: GLOB.useAnimations

                ColorAnimation {
                }

            }

        }

        Text {
            text: wrapper.composer
            y: NOO.factor() * 1.5
            color: wrapArea.containsMouse ? activPal.text : NOO.alpha(activPal.text, 150)
            maximumLineCount: 1

            anchors {
                right: parent.right
                rightMargin: NOO.factor() / 4
            }

            Behavior on color {
                enabled: GLOB.useAnimations

                ColorAnimation {
                }

            }

        }
        // TmelodyWrapper

        states: State {
            when: wrapArea.held

            ParentChange {
                target: wrapper
                parent: melListView.viewRoot
            }

            AnchorChanges {
                target: wrapper

                anchors {
                    horizontalCenter: undefined
                    verticalCenter: undefined
                }

            }

        }

    }

    DropArea {
        onEntered: {
            if (drag.source.nr !== wrapArea.nr) {
                if (drag.source.nr === melListView.currentMelody)
                    melListView.currentMelody = wrapper.nr;

                melListView.moveMelody(drag.source.nr, wrapArea.nr);
            }
        }

        anchors {
            fill: parent
            margins: 10
        }

    }

}
