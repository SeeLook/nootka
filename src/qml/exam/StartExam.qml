/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../level"
import Nootka 1.0
import Nootka.Exam 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TstartExamItem {
    id: startDialog

    // level selector visible only for tablet or landscape layout
    property bool hideSelector: NOO.isAndroid() ? width / (NOO.fingerPixels() / 0.7) < 15 : height > width
    property var exExGotIt: null
    property bool remaindMeLater: GLOB.gotIt("examOrExercise", false)

    function examOrExerGotIt() {
        if (!exExGotIt)
            exExGotIt = Qt.createComponent("qrc:/gotit/ExamOrExercise.qml").createObject(startDialog, {
            "remaindChecked": remaindMeLater
        });

        exExGotIt.open();
    }

    function start(action, argument) {
        if (userNameIn.text === "") {
            noNameAnim.running = true;
            noNamePopup.open();
            return ;
        }
        GLOB.student = userNameIn.text;
        GLOB.isExam = true;
        nootkaWindow.executor.init(action, argument);
        dialLoader.close();
    }

    width: parent.width
    height: parent.height
    onContinueExam: start(Texecutor.ContinueExam, examFile)
    Component.onCompleted: {
        dialLoader.stdButtons = DialogButtonBox.NoButton;
        if (!NOO.isAndroid())
            dialLoader.title = qsTr("Start exercises or an exam");

    }
    Component.onDestruction: {
        if (exExGotIt)
            GLOB.setGotIt("examOrExercise", exExGotIt.remaindChecked);

    }

    Column {
        id: startCol

        width: parent.width
        height: parent.height
        spacing: NOO.isAndroid() ? Math.min(NOO.factor() * 2, (startDialog.height - upperRow.height - infoText.height - levelWrapper.height - grid1.height - grid2.height) / 4.5) : NOO.factor()
        topPadding: NOO.factor() / 4

        Row {
            id: upperRow

            spacing: NOO.factor() * (NOO.isAndroid() || hideSelector ? 2 : 4)
            anchors.horizontalCenter: parent.horizontalCenter

            TiconButton {
                anchors.verticalCenter: parent.verticalCenter
                pixmap: NOO.pix("help")
                iconHeight: startDialog.height / 15
                text: NOO.TR("QShortcut", "Help")
                onClicked: examOrExerGotIt()
                color: enabled ? Qt.tint(activPal.button, NOO.alpha("orange", NOO.isAndroid() ? 40 : 0)) : disdPal.button
            }

            Grid {
                spacing: NOO.factor() * (NOO.isAndroid() || hideSelector ? 0.5 : 1)
                anchors.verticalCenter: parent.verticalCenter
                horizontalItemAlignment: Grid.AlignHCenter
                verticalItemAlignment: Grid.AlignVCenter
                columns: hideSelector ? 1 : 2

                Text {
                    text: qsTr("student name:")
                    color: activPal.text
                }

                TtextField {
                    id: userNameIn

                    placeholderText: qsTr("Enter your name or nick-name.")
                    font.pixelSize: NOO.factor()
                    maximumLength: 40
                    width: NOO.factor() * (NOO.isAndroid() ? 19 : 25)
                    horizontalAlignment: TextInput.AlignHCenter
                    text: GLOB.student
                }

            }

            TiconButton {
                anchors.verticalCenter: parent.verticalCenter
                pixmap: NOO.pix("levelCreator")
                iconHeight: startDialog.height / 15
                text: NOO.TR("TlevelCreatorItem", "Level creator").replace(" ", "\n")
                color: enabled ? Qt.tint(activPal.button, NOO.alpha("teal", NOO.isAndroid() ? 40 : 0)) : disdPal.button
                onClicked: {
                    dialLoader.close();
                    nootkaWindow.showDialog(3); // Nootka.LevelCreator
                }
            }

        }

        Text {
            id: infoText

            text: qsTr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file.")
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.9
            horizontalAlignment: Text.AlignHCenter
            color: activPal.text
            wrapMode: Text.WordWrap

            font {
                bold: true
                pixelSize: NOO.factor() * 0.8
            }

        }

        Item {
            id: levelWrapper

            anchors.horizontalCenter: parent.horizontalCenter
            width: childrenRect.width
            height: childrenRect.height

            LevelsSelector {
                id: selector

                parent: hideSelector ? selPopLoader.item.contentItem : levelWrapper
                width: startDialog.width - NOO.factor() * (hideSelector ? 2 : 1)
                height: hideSelector ? startDialog.height * 0.7 : startDialog.height - upperRow.height - grid1.height - grid2.height - infoText.height - (NOO.isAndroid() ? 4 : 6) * startCol.spacing
            }

        }

        Grid {
            id: grid1

            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)
            columns: hideSelector ? 1 : 2

            Tframe {
                width: startDialog.width / (hideSelector ? 1 : 2) - NOO.factor()

                Row {
                    spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                    TiconButton {
                        enabled: selector.levelId > -1 || prevLevelName() !== ""
                        iconHeight: startDialog.height / 15
                        pixmap: NOO.pix("practice")
                        text: qsTr("Start exercise on level:")
                        onClicked: start(Texecutor.StartExercise, selector.levelId > -1 ? selector.currentLevelVar() : prevLevel())
                        color: enabled ? Qt.tint(activPal.button, NOO.alpha("green", NOO.isAndroid() ? 40 : 0)) : disdPal.button
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "<b>" + (selector.levelId > -1 ? selector.levelName(selector.levelId) : (prevLevelName() === "" ? qsTr("No level was selected!") : prevLevelName())) + "</b>"
                        color: activPal.text
                        font.pixelSize: NOO.factor() * 0.8
                    }

                }

            }

            Tframe {
                width: startDialog.width / (hideSelector ? 1 : 2) - NOO.factor()

                Row {
                    spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                    TiconButton {
                        enabled: selector.levelId !== -1
                        iconHeight: startDialog.height / 15
                        pixmap: NOO.pix("exam")
                        text: qsTr("Pass new exam on level:")
                        onClicked: start(Texecutor.NewExam, selector.currentLevelVar())
                        color: enabled ? Qt.tint(activPal.button, NOO.alpha("yellow", NOO.isAndroid() ? 40 : 0)) : disdPal.button
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "<b>" + (selector.levelId > -1 ? selector.levelName(selector.levelId) : qsTr("No level was selected!"))
                        color: activPal.text
                        font.pixelSize: NOO.factor() * 0.8
                    }

                }

            }

        }

        Grid {
            id: grid2

            width: startDialog.width - NOO.factor()
            spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)
            columns: hideSelector ? 1 : 2

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            Tframe {
                id: lastExamFrame

                width: hideSelector ? startDialog.width - NOO.factor() : parent.width - contFrame.width - exitFrame.width - 2 * NOO.factor()

                Row {
                    spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                    TiconButton {
                        id: lastExamButt

                        iconHeight: startDialog.height / 15
                        pixmap: NOO.pix("exam")
                        text: qsTr("Latest exam")
                        enabled: lastExamFile !== ""
                        onClicked: start(Texecutor.ContinueExam, lastExamFile)
                        color: enabled ? Qt.tint(activPal.button, NOO.alpha("orange", NOO.isAndroid() ? 40 : 0)) : disdPal.button
                    }

                    Text {
                        width: lastExamFrame.width - lastExamButt.width - 3 * NOO.factor()
                        anchors.verticalCenter: parent.verticalCenter
                        text: lastExamButt.enabled ? lastExamFile : NOO.TR("QPrintSettingsOutput", "None")
                        color: activPal.text
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        maximumLineCount: 2
                        elide: Text.ElideRight

                        font {
                            pixelSize: NOO.factor() * 0.8
                            bold: true
                        }

                    }

                }

            }

            Row {
                spacing: NOO.factor()

                Tframe {
                    id: contFrame

                    border.width: 0

                    Row {
                        TiconButton {
                            iconHeight: startDialog.height / 15
                            pixmap: NOO.pix("exam")
                            text: qsTr("Select an exam to continue") + "   ⋮"
                            onClicked: menu.open()
                            color: enabled ? Qt.tint(activPal.button, NOO.alpha("orange", NOO.isAndroid() ? 40 : 0)) : disdPal.button
                        }

                    }

                }

                Tframe {
                    id: exitFrame

                    border.width: 0

                    Row {
                        TiconButton {
                            iconHeight: startDialog.height / 15
                            pixmap: NOO.pix("exit")
                            text: NOO.TR("QShortcut", "Exit")
                            onClicked: dialLoader.close()
                            color: enabled ? Qt.tint(activPal.button, NOO.alpha("red", NOO.isAndroid() ? 60 : 0)) : disdPal.button
                        }

                    }

                }

            }

        }

    }

    Tmenu {
        id: menu

        width: Math.min(NOO.factor() * 25, startDialog.width * 0.8)
        height: Math.min(startDialog.height * 0.8, contentItem.contentHeight)
        y: (startDialog.height - height) / 2
        x: (startDialog.width - width) / 2
        modal: true

        contentItem: ListView {
            clip: true
            model: recentModel

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            delegate: MenuButton {
                width: menu.width
                action: modelData
                onClicked: menu.close()

                Rectangle {
                    width: parent.width
                    height: index === recentModel.length - 1 ? 0 : 1
                    color: activPal.text
                    y: parent.height - 1
                }

            }

        }

    }

    TpopupDialog {
        id: noNamePopup

        bgColor: Qt.tint(activPal.window, NOO.alpha("red", 20))
        rejectButton.text: NOO.TR("QShortcut", "OK")
        rejectButton.pixmap: NOO.pix("check")
        acceptButton.visible: false
        width: noNameTxt.width + NOO.factor() * 2
        height: noNameTxt.height + NOO.factor() * 5
        onRejected: {
            noNameAnim.running = false;
            userNameIn.bg.color = activPal.base;
        }

        border {
            color: "red"
            width: NOO.factor() / 4
        }

        Text {
            id: noNameTxt

            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            font.pixelSize: NOO.factor() * 1.5
            text: qsTr("Give an user name!")
        }

    }

    Loader {
        id: selPopLoader

        active: hideSelector

        sourceComponent: TpopupDialog {
            padding: 0
            rejectButton.visible: false
            parent: startDialog
            width: selector.width + NOO.factor()
            height: selector.height + NOO.factor() * 4
        }

    }

    Loader {
        active: hideSelector

        sourceComponent: Tframe {
            parent: levelWrapper

            Row {
                spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                TiconButton {
                    iconHeight: startDialog.height / 15
                    pixmap: NOO.pix("nootka-level")
                    text: NOO.TR("LevelsPage", "Level name:")
                    onClicked: selPopLoader.item.open()
                    color: enabled ? Qt.tint(activPal.button, NOO.alpha(activPal.highlight, NOO.isAndroid() ? 40 : 0)) : disdPal.button
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "<b>" + (selector.levelId > -1 ? selector.levelName(selector.levelId) : qsTr("No level was selected!"))
                    color: activPal.text
                    font.pixelSize: NOO.factor() * 0.8
                }

            }

        }

    }

    SequentialAnimation {
        id: noNameAnim

        loops: Animation.Infinite

        ColorAnimation {
            target: userNameIn.bg
            property: "color"
            to: "red"
            duration: 500
        }

        PauseAnimation {
            duration: 300
        }

        ColorAnimation {
            target: userNameIn.bg
            property: "color"
            to: activPal.base
            duration: 500
        }

        PauseAnimation {
            duration: 300
        }

    }

    Timer {
        interval: 500
        running: true
        onTriggered: {
            if (remaindMeLater)
                examOrExerGotIt();

        }
    }

}
