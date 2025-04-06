/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: nootkaWindow

    property bool topToBott: height > width
    property real shortEdge: Math.min(height, width * (GLOB.instrument.isSax ? 0.85 : 1))
    property alias mainMenu: mainMenu

    // private
    property var noteName: null
    property var examResults: null
    property var executor: null
    property var dialogLoader: null
    property var tip: null
    property var analyzeWindow: null
    property var sndInf: null
    property var notesBarItem: null

    function createStatus() {
        Qt.createComponent("qrc:/StatusTip.qml").createObject(NOO.isAndroid() ? nootkaWindow.contentItem : nootkaWindow.contentItem.parent);
    }

    function checkSingleMode() {
        if (GLOB.singleNoteMode) {
            if (!noteName) {
                var c = Qt.createComponent("qrc:/NoteName.qml");
                noteName = c.createObject(scoreWrap, {
                    "score": score
                });
            }
        } else {
            if (noteName)
                noteName.destroy();

        }
    }

    function showDialog(page) {
        if (!dialogLoader)
            dialogLoader = Qt.createComponent("qrc:/DialogLoader.qml").createObject(nootkaWindow);

        if (page === Nootka.Charts) {
            // dialogLoader above registers QML types in C++ - keep that order!
            analyzeWindow = Qt.createComponent("qrc:/charts/AnalyzeDialog.qml").createObject(nootkaWindow);
            return ;
        }
        if (page === Nootka.ScoreImport) {
            Qt.createComponent("qrc:/score/MelodyImport.qml").createObject(dialogLoader.page === Nootka.LevelCreator ? dialogLoader : nootkaWindow);
            return ;
        }
        dialogLoader.page = page;
    }

    function askForSupport() {
        Qt.createComponent("qrc:/about/SupportPopup.qml").createObject(contentItem.parent);
    }

    function newerVerPop() {
        var p = analyzeWindow ? analyzeWindow : (dialogLoader && dialogLoader.page ? dialogLoader.contentItem : nootkaWindow);
        return Qt.createComponent("qrc:/level/NewerVersionPopup.qml").createObject(p);
    }

    function audioAnalyze() {
        Qt.createComponent("qrc:/nootini/AudioAnalyze.qml").createObject(nootkaWindow);
        nootkaWindow.title = "Nootini - pitch detection analyzer";
    }

    visible: true
    title: "Nootka"
    color: activPal.window
    visibility: NOO.isAndroid() && GLOB.fullScreen() ? ApplicationWindow.FullScreen : ApplicationWindow.AutomaticVisibility
    width: GLOB.geometry.width
    height: GLOB.geometry.height
    x: GLOB.geometry.x
    y: GLOB.geometry.y
    header: mainMenu.toolBar

    Component.onCompleted: {
        NOO.mainScore = score;
        checkSingleMode();
        if (GLOB.gotIt("soundInfo", true)) {
            sndInf = Qt.createComponent("qrc:/gotit/SoundInfo.qml").createObject(nootkaWindow.contentItem.parent);
            sndInf.remaindChecked = true;
            sndInf.closed.connect(function() {
                GLOB.setGotIt("soundInfo", sndInf.remaindChecked);
                sndInf.destroy();
                createStatus(); // create status tip only after pitch detection info
            });
        } else {
            createStatus();
        }
    }

    onClosing: close => {
        if (NOO.isAndroid() && dialogLoader && dialogLoader.visible) {
            close.accepted = false;
            dialogLoader.close();
        }
        if (executor) {
            if (!executor.closeNootka())
                close.accepted = false;
        }
        if (close.accepted)
            GLOB.geometry = Qt.rect(x, y, width, height);

    }

    SystemPalette {
        id: activPal

        property color dimText: Qt.tint(activPal.base, NOO.alpha(activPal.text, 150))
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: disdPal
        colorGroup: SystemPalette.Disabled
    }

    Connections {
        target: NOO
        function onScoreActTriggered() : void { mainMenu.open() }
        function onMelodyActTriggered() : void { mainMenu.melodyOpen() }
        function onSettingsActTriggered() : void { showDialog(Nootka.Settings) }
        function onLevelActTriggered() : void { showDialog(Nootka.LevelCreator) }
        function onChartsActTriggered() : void { showDialog(Nootka.Charts) }
        function onExamActTriggered() : void { showDialog(Nootka.ExamStart) }
        function onAboutActTriggered() : void { showDialog(Nootka.About) }
    }

    MainMenu {
        id: mainMenu
    }

    Item {
        id: scoreWrap

        property real insHi: GLOB.instrument.getItemHeight(shortEdge)

        y: examResults ? examResults.height + 2 : 0
        height: nootkaWindow.contentItem.height - (GLOB.instrument.isSax ? (GLOB.singleNoteMode ? insHi / 7 : 0) : insHi) - (examResults ? examResults.height + 2 : 0) - (notesBarItem ? notesBarItem.height : 0)
        width: parent.width * (GLOB.instrument.isSax ? 0.85 : 1)
        z: 5
        transformOrigin: Item.Top

        MainScore {
            id: score

            z: 5
            height: parent.height / (GLOB.singleNoteMode && topToBott ? 2 : 1)
            width: parent.width / (GLOB.singleNoteMode && !topToBott ? 2 : 1)
        }
    }

    Instrument {
        id: instrument
        score: score
    }

    Connections {
        target: GLOB
        function onSingleNoteModeChanged() : void { checkSingleMode() }
        function onIsExamChanged() : void {
            if (GLOB.isExam) {
                if (!executor)
                    executor = Qt.createComponent("qrc:/exam/ExamExecutor.qml").createObject(nootkaWindow);
                if (!examResults)
                    examResults = Qt.createComponent("qrc:/exam/ExamResults.qml").createObject(nootkaWindow.contentItem);
            } else {
                if (examResults)
                    examResults.destroy();
            }
        }
        function onWantOpenFile(fileName: string) : void {
            showDialog(Nootka.NoDialog);
            dialogLoader.openFile(fileName);
        }
        function onShowNotesDiffChanged() : void {
            if (GLOB.showNotesDiff) {
                if (!notesBarItem)
                    notesBarItem = Qt.createComponent("qrc:/sound/NotesDiffBar.qml").createObject(nootkaWindow.contentItem);
            } else {
                if (notesBarItem)
                    notesBarItem.destroy();
            }
        }
    }

}
