/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ToolBar {
    id: toolBar

    property alias scoreAct: scoreAct
    property alias melodyAct: melodyAct
    property alias examActions: examRepeater.model

    height: settAct.height
    z: 6
    padding: 0

    Row {
        spacing: toolBar.width / 200

        HeadButton {
            id: settAct

            taction: NOO.settingsAct
            visible: !GLOB.isExam
        }

        HeadButton {
            taction: NOO.levelAct
            visible: !GLOB.isExam
        }

        HeadButton {
            id: scoreAct

            taction: NOO.scoreAct
            visible: !GLOB.singleNoteMode
        }

        HeadButton {
            id: melodyAct

            taction: NOO.melodyAct
            visible: !GLOB.singleNoteMode && !GLOB.isExam
        }

        HeadButton {
            id: chartAct

            taction: NOO.chartsAct
            visible: !GLOB.isExam
        }

        HeadButton {
            taction: NOO.examAct
            visible: !GLOB.isExam
        }

        Repeater {
            id: examRepeater

            HeadButton {
                taction: modelData
                visible: taction && taction.enabled
            }

        }

    }

    Connections {
        property var pitchView

        target: SOUND
        function onInitialized() : void {
            if (!pitchView) {
                var c = Qt.createComponent("qrc:/PitchView.qml");
                pitchView = c.createObject(toolBar);
                pitchView.x = Qt.binding(function() {
                    return (label.visible ? label.x : toolBar.width) - toolBar.width * 0.433;
                });
            }
        }
    }

    NootkaLabel {
        id: label

        visible: !executor
        anchors.right: parent.right
        height: toolBar.height
        onClicked: NOO.aboutAct.trigger()
        onEntered: NOO.setStatusTip(NOO.TR("TaboutNootka", "About Nootka") + "...<br>" + NOO.TR("QShortcut", "Help")
                                            + ", " + NOO.TR("TaboutNootka", "Authors").toLowerCase() + ", " + NOO.TR("TaboutNootka", "License").toLowerCase(),
                                    Item.TopLeft)
        onExited: NOO.setStatusTip("", Item.TopLeft)
    }

    background: TipRect {
        anchors.fill: parent
        color: activPal.window
        radius: 0
    }

}
