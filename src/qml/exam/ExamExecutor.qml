/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Exam 1.0
import QtQuick 2.12

pragma ComponentBehavior: Bound

Texecutor {
    id: executor

    //private
    property var examSettDialog: null
    property var helpGotIt: null

    parent: NOO.isAndroid() ? nootkaWindow.contentItem : nootkaWindow.contentItem.parent
    z: 100
    anchors.fill: parent
    onTitleChanged: nootkaWindow.title = title
    onExamActionsChanged: {
        if (!NOO.isAndroid())
            nootkaWindow.mainMenu.toolBar.examActions = examActions;

    }
    onExamSummary: nootkaWindow.showDialog(Nootka.ExamSummary)
    onShowSettings: {
        if (!examSettDialog) {
            examSettDialog = Qt.createComponent("qrc:/exam/ExamSettingsDialog.qml").createObject(executor, {
                "mode": isExercise ? 2 : 1
            });
            examSettDialog.accepted.connect(settingsAccepted);
            examSettDialog.closed.connect(function() {
                examSettDialog.destroy();
            });
        }
    }
    onWantMessage: {
        var ms = Qt.createComponent("qrc:/Tmessage.qml").createObject(nootkaWindow.contentItem.parent, {
            "caption": caption,
            "message": message,
            "accent": accent
        });
        ms.closed.connect(afterMessage);
    }
    onExecDiscarded: {
        console.log("[ExamExecutor] Executor discarded, deleting it");
        GLOB.isExam = false;
        nootkaWindow.executor.destroy();
    }
    onShowHelp: {
        if (!helpGotIt)
            helpGotIt = Qt.createComponent("qrc:/gotit/ExamFlow.qml").createObject(executor, {
            "remaindChecked": showExamHelp
        });

        helpGotIt.open();
        helpGotIt.closed.connect(afterMessage);
    }
    onWantSuggestPopup: Qt.createComponent("qrc:/exam/TsuggestExam.qml").createObject(executor, {
        "entireVisible": showEntire
    })
    Component.onDestruction: {
        if (helpGotIt)
            GLOB.setGotIt("examFlow", helpGotIt.remaindChecked);

    }

    Connections {
        target: tipHandler
        function onWantStartTip(text: string, color: color, pos: point) : void {
            let s = Qt.createComponent("qrc:/exam/ExamTip.qml");
            tipHandler.startTip = s.createObject(executor, {
                text: text,
                offX: pos.x,
                offY: pos.y,
                bg: color
            });
        }

        function onWantQuestionTip(text: string, pos: point) : void {
            let s = Qt.createComponent("qrc:/exam/QuestionTip.qml");
            tipHandler.questionTip = s.createObject(executor, {
                text: text,
                offX: pos.x,
                offY: pos.y
            });
        }

        function onWantConfirmTip(text: string, color: color, pos: point) : void {
            let s = Qt.createComponent("qrc:/exam/ExamTip.qml");
            tipHandler.confirmTip = s.createObject(nootkaWindow, {
                text: text,
                offX: pos.x,
                offY: pos.y,
                bg: color,
                showExit: !NOO.isAndroid()
            });
        }

        function onWantResultTip(text: string, color: color) : void {
            let r = Qt.createComponent("qrc:/exam/ResultTip.qml");
            tipHandler.resultTip = r.createObject(nootkaWindow, {
                text: text,
                color: color
            });
        }

        function onWantTryAgainTip() : void {
            let a = Qt.createComponent("qrc:/exam/ResultTip.qml");
            tipHandler.tryAgainTip = a.createObject(nootkaWindow, {
                text: qsTranslate("TtipHandler", "Try again!"),
                color: GLOB.wrongColor,
                targetY: Math.min(executor.width, executor.height) / 12
            });
        }

        function onWantWhatNextTip(text: string, color: color, pos: point) : void {
            let s = Qt.createComponent("qrc:/exam/ExamTip.qml");
            tipHandler.whatNextTip = s.createObject(nootkaWindow, {
                text: text,
                offX: pos.x,
                offY: pos.y,
                bg: color
            });
        }

        function onWantCertificate() : void {
            tipHandler.certTip = Qt.createComponent("qrc:/exam/Certificate.qml").createObject(nootkaWindow.contentItem.parent);
        }

    }

}
