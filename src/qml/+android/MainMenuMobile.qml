/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import Qt5Compat.GraphicalEffects
import QtQuick 2.12
import QtQuick.Controls 2.12
import "sound"

TmobileMenu {
    id: root

    property Item toolBar: null // fake, for main window information
    property alias drawer: mainDrawer
    // private
    property var scoreMenu: null
    property var tempoMenu: null
    property var clearPopup: null

    function open() {
        if (!scoreMenu) {
            var c = Qt.createComponent("qrc:/ScoreMenuContent.qml");
            scoreMenu = c.createObject(nootkaWindow.contentItem.parent, {
                "width": NOO.factor() * 15,
                "z": -1
            });
        }
        scoreMenu.open();
    }

    onClicked: mainDrawer.open()
    parent: nootkaWindow.contentItem
    z: 550
    width: fingerPixels()
    height: fingerPixels() * 1.3
    onWantClearScore: {
        if (!clearPopup)
            clearPopup = clearComp.createObject(nootkaWindow.contentItem);

        clearPopup.open();
    }
    onFlyClicked: {
        if (currentFly)
            currentFly.taction.trigger();

    }

    Rectangle {
        id: bg

        width: fingerPixels() * 0.7
        height: fingerPixels() * 1.3
        x: fingerPixels() / 10
        y: fingerPixels() / 10
        color: NOO.alpha(activPal.highlight, pressed ? 255 : 25)
        radius: fingerPixels() / 10

        Column {
            width: parent.width
            spacing: bg.width / 4
            topPadding: spacing

            Rectangle {
                width: bg.width / 4
                height: width
                radius: width / 2
                anchors.horizontalCenter: parent.horizontalCenter
                color: SOUND.playing ? "lime" : "black"
            }

            Rectangle {
                width: bg.width / 4
                height: width
                radius: width / 2
                anchors.horizontalCenter: parent.horizontalCenter
                color: "black"
            }

            Rectangle {
                id: pitchDot

                width: bg.width / 4
                height: width
                radius: width / 2
                anchors.horizontalCenter: parent.horizontalCenter
                color: SOUND.listening ? "mediumturquoise" : "black"
            }

        }

    }

    Taction {
        id: tunerAct

        text: NOO.TR("TunerDialog", "Nooter - Nootka tuner").replace("-", "<br><font size=\"1\">") + "</font>"
        icon: "fork"
        onTriggered: {
            nootkaWindow.showDialog(Nootka.Tuner);
            SOUND.startListen();
        }
    }

    Connections {
        target: tempoAct
        onTriggered: {
            if (!tempoMenu)
                tempoMenu = Qt.createComponent("qrc:/sound/TempoMenu.qml").createObject(nootkaWindow.contentItem);

            tempoMenu.open();
        }
    }

    Drawer {
        id: mainDrawer

        property NootkaLabel label: null

        width: NOO.factor() * 20
        height: nootkaWindow.height
        onVisibleChanged: {
            if (visible) {
                if (!drawerLoad.active)
                    drawerLoad.active = true;

                label.bgColor = NOO.randomColor();
                SOUND.stopListen();
            } else {
                if (!dialogLoader || !dialogLoader.visible)
                    SOUND.startListen();

            }
        }

        Loader {
            id: drawerLoad

            active: false
            anchors.fill: parent

            sourceComponent: Component {
                Flickable {
                    anchors.fill: parent
                    clip: true
                    contentHeight: drawerColumn.height

                    Column {
                        id: drawerColumn

                        property var drawer: mainDrawer

                        width: parent.width
                        spacing: fingerPixels() / 8

                        NootkaLabel {
                            id: nooLabel

                            property int currentItem: 0

                            height: NOO.factor() * 7.91016 // (logo ratio) 0.3955078125 * 20
                            enabled: !GLOB.isExam
                            clip: true
                            onClicked: {
                                mainDrawer.close();
                                NOO.aboutAct.trigger();
                            }
                            Component.onCompleted: mainDrawer.label = this

                            ListModel {
                                id: paneModel

                                ListElement {
                                    txt: "About"
                                    img: "about"
                                }

                                ListElement {
                                    txt: "Help"
                                    img: "help"
                                }

                                ListElement {
                                    txt: "Authors"
                                    img: "author"
                                }

                                ListElement {
                                    txt: "License"
                                    img: "license"
                                }

                                ListElement {
                                    txt: "Support"
                                    img: "support"
                                }

                                ListElement {
                                    txt: "Changes"
                                    img: "changes"
                                }

                            }

                            Rectangle {
                                id: item

                                width: parent.height / 2
                                height: width * 1.4
                                x: parent.width - width - NOO.factor() / 4
                                y: -height
                                radius: width / 8
                                color: Qt.rgba(0, 0, 0, 0.8)

                                Image {
                                    id: paneImg

                                    source: NOO.pix("pane/" + paneModel.get(nooLabel.currentItem).img)
                                    height: parent.width
                                    width: height * (sourceSize.width / sourceSize.height)
                                    visible: false
                                }

                                Colorize {
                                    anchors.fill: paneImg
                                    source: paneImg
                                    hue: 0
                                    lightness: 0.5
                                    saturation: 0
                                }

                                Text {
                                    color: nooLabel.bgColor
                                    text: NOO.TR(nooLabel.currentItem === 1 ? "QShortcut" : "TaboutNootka", paneModel.get(nooLabel.currentItem).txt)
                                    width: parent.width
                                    horizontalAlignment: Text.AlignHCenter
                                    fontSizeMode: Text.Fit
                                    minimumPixelSize: NOO.factor() / 2

                                    anchors {
                                        horizontalCenter: parent.horizontalCenter
                                        bottom: parent.bottom
                                    }

                                }

                                SequentialAnimation {
                                    loops: paneModel.count
                                    running: true

                                    PauseAnimation {
                                        duration: 500
                                    }

                                    PropertyAnimation {
                                        target: item
                                        property: "y"
                                        duration: 250
                                        to: (item.parent.height - item.height) / 2
                                    }

                                    PauseAnimation {
                                        duration: 1500
                                    }

                                    PropertyAnimation {
                                        target: item
                                        property: "y"
                                        duration: 500
                                        to: item.parent.height + item.height
                                    }

                                    ScriptAction {
                                        script: {
                                            item.y = -item.height;
                                            if (nooLabel.currentItem < paneModel.count - 1)
                                                nooLabel.currentItem++;

                                        }
                                    }

                                }

                            }

                        }

                        Repeater {
                            model: examActions

                            MenuButton {
                                action: modelData
                                visible: action && action.enabled

                                Rectangle {
                                    width: NOO.factor() * 3
                                    height: parent.height
                                    z: -1
                                    color: action ? NOO.alpha(action.bgColor, 50) : "transparent"
                                }

                            }

                        }

                        MenuButton {
                            visible: !executor || executor.showPitchView
                            action: pitchDetectAct
                        }

                        MenuButton {
                            visible: !GLOB.singleNoteMode
                            action: tempoAct

                            Text {
                                property var beatModel: ["\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3"]

                                x: parent.width - width - NOO.factor() / 2
                                y: parent.height * -0.55
                                text: beatModel[SOUND.beatUnit] + "=" + SOUND.tempo

                                font {
                                    family: "Scorek"
                                    pixelSize: parent.height * 0.6
                                }

                            }

                        }

                        MenuButton {
                            visible: !GLOB.isExam
                            action: NOO.examAct
                        }

                        MenuButton {
                            visible: !GLOB.isExam
                            action: NOO.levelAct
                        }

                        MenuButton {
                            visible: !GLOB.singleNoteMode
                            action: NOO.scoreAct
                        }

                        MenuButton {

                            action: Taction {
                                text: NOO.settingsAct.text
                                icon: GLOB.isExam ? "exam-settings" : "systemsettings"
                                onTriggered: {
                                    if (GLOB.isExam)
                                        executor.settingsAct.trigger();
                                    else
                                        NOO.settingsAct.trigger();
                                }
                            }

                        }

                        MenuButton {
                            visible: !GLOB.isExam
                            action: tunerAct

                            Text {
                                x: parent.width - width - NOO.factor() / 4
                                y: (parent.height - height) / 2
                                text: "A = " + GLOB.midAfreq

                                font {
                                    pixelSize: parent.height * 0.55
                                    bold: true
                                }

                            }

                        }

                        MenuButton {
                            onClicked: nootkaWindow.close()

                            action: Taction {
                                icon: "close"
                                text: NOO.TR("QShortcut", "Close")
                            }

                        }

                    }

                }

            }

        }

    }

    Component {
        id: clearComp

        TpopupDialog {
            width: footWidth + NOO.factor()
            height: clearCol.height + NOO.factor() * 4
            bgColor: Qt.tint(activPal.window, NOO.alpha("orange", 20))
            onAccepted: score.clearScoreAct.trigger()

            border {
                color: "orange"
                width: NOO.factor() / 4
            }
            //modal: true

            Column {
                id: clearCol

                anchors.horizontalCenter: parent.horizontalCenter

                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: NOO.pix("clear-score")
                    height: NOO.factor() * 4
                    width: height
                }

                Text {
                    text: NOO.TR("TscoreObject", "Delete all notes")
                    color: activPal.text
                }

            }

        }

    }

    Connections {
        target: SOUND
        onVolumeKeyPressed: nootkaWindow.showDialog(Nootka.Tuner)
        onListeningChanged: pitchDot.scale = 1
    }

    FlyItem {
        taction: fly1act
        index: 0
    }

    FlyItem {
        taction: fly2act
        index: 1
    }

    FlyItem {
        taction: fly3act
        index: 2
    }

    FlyItem {
        taction: fly4act
        index: 3
    }

    FlyItem {
        taction: fly5act
        index: 4
    }

    Timer {
        repeat: true
        interval: 75
        running: SOUND.listening
        onTriggered: pitchDot.scale = 1 + SOUND.inputVol()
    }

}
