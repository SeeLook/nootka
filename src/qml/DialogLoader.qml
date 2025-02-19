/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs
import QtQuick.Window 2.12

Dialog {
    id: dialLoader

    property int stdButtons: DialogButtonBox.NoButton
    property int page: 0
    property alias buttonBox: box
    property alias dialogObj: dialogObj
    property var currentDialog: null

    signal apply()
    signal help()

    function openFile(file) {
        dialogObj.openFile(file);
    }

    width: nootkaWindow.width
    height: nootkaWindow.height
    padding: 0

    onPageChanged: {
        if (page > 0) {
            if (NOO.isAndroid())
                mainMenu.drawer.interactive = false;

            dialLoader.width = nootkaWindow.width
            dialLoader.height = nootkaWindow.height
            open(); // do it it first, to initialize size at first time
            switch (page) {
            case Nootka.Settings:
                currentDialog = Qt.createComponent("qrc:/TsettingsDialog.qml").createObject(container);
                break;
            case Nootka.About:
                currentDialog = Qt.createComponent("qrc:/TaboutNootka.qml").createObject(container);
                break;
            case Nootka.LevelCreator:
                currentDialog = Qt.createComponent("qrc:/LevelCreator.qml").createObject(container);
                break;
            case Nootka.ExamStart:
                currentDialog = Qt.createComponent("qrc:/exam/StartExam.qml").createObject(container);
                break;
            case Nootka.ExamSummary:
                currentDialog = Qt.createComponent("qrc:/exam/ExamSummary.qml").createObject(container);
                break;
            case Nootka.Tuner:
                currentDialog = Qt.createComponent("qrc:/sound/TunerDialog.qml").createObject(container);
                break;
            case Nootka.MelodyGenerator:
                currentDialog = Qt.createComponent("qrc:/score/MelGenDialog.qml").createObject(container);
                break;
            case Nootka.Updater:
                currentDialog = Qt.createComponent("qrc:/updater/TupdateSummary.qml").createObject(container);
                dialLoader.title = currentDialog.titleText;
                dialLoader.stdButtons = DialogButtonBox.Ok;
                break;
            }
            SOUND.stopListen();
            SOUND.stopPlaying();
        }
    }
    onVisibleChanged: {
        if (visible === false && currentDialog) {
            if (NOO.isAndroid())
                mainMenu.drawer.interactive = true;

            if (page !== Nootka.ExamStart)
                SOUND.startListen();

            currentDialog.destroy();
            currentDialog = null;
            page = 0;
        }
    }
    onApply: {
        if (currentDialog)
            currentDialog.apply();
        close();
    }
    onReset: {
        if (currentDialog)
            currentDialog.reset();
    }
    onAccepted: {
        if (currentDialog)
            currentDialog.accepted();
    }
    onHelp: {
        if (currentDialog)
            currentDialog.help();
    }

    TdialogObject {
        id: dialogObj

        onContinueExam: {
            GLOB.isExam = true;
            nootkaWindow.executor.init(1, examFile);
            close();
        }
        onOpenLevel: levelFile => {
            page = Nootka.LevelCreator;
            currentDialog.openLevel(levelFile);
        }
    }

    contentItem: Column {
        width: dialLoader.width
        height: dialLoader.height
        spacing: NOO.factor() / 2

        Rectangle {
            id: container

            width: dialLoader.width
            height: dialLoader.height - (box.visible ? NOO.factor() * 4 : 0) - header.height
            color: activPal.window
        }

        DialogButtonBox {
            id: box

            z: 5
            standardButtons: dialLoader.stdButtons
            visible: dialLoader.stdButtons !== DialogButtonBox.NoButton
            width: dialLoader.width
            height: NOO.factor() * 3.5
            spacing: NOO.factor()
            alignment: Qt.AlignVCenter
            onClicked: button => {
                switch (button.DialogButtonBox.buttonRole) {
                case DialogButtonBox.AcceptRole:
                    dialLoader.accepted();
                    break;
                case DialogButtonBox.HelpRole:
                    dialLoader.help();
                    break;
                case DialogButtonBox.ResetRole:
                    dialLoader.reset();
                    break;
                case DialogButtonBox.ApplyRole:
                    dialLoader.apply();
                    break;
                case DialogButtonBox.ActionRole:
                    break;
                default:
                    dialLoader.close();
                }
            }

            delegate: TiconButton {
                width: Math.min(box.width / 2, (box.width - NOO.factor() * (box.count + 1)) / box.count)
                pixmap: NOO.pix(dialogObj.buttonRoleIcon(DialogButtonBox.buttonRole))
                color: Qt.tint(activPal.button, NOO.alpha(dialogObj.buttinRoleColor(DialogButtonBox.buttonRole), NOO.isAndroid() ? 20 : 0))
            }

            background: Rectangle {
                anchors.fill: parent
                color: NOO.isAndroid() ? activPal.base : Qt.darker(activPal.window, 1.1)
            }

        }

    }

}
