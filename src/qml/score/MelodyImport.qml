/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../score"
import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import Score 1.0

Window {
    // It covers both cases: when model are ready before dialog and ready only after this onCompleted

    id: importWindow

    property alias multiSelect: melImport.multiSelect
    property var dividePop: null
    property var transPop: null
    property var allChordsPop
    property var chordView: null

    function showChord(chordIt) {
        if (!chordView)
            chordView = chordComp.createObject(importWindow);

        chordView.chordIt = chordIt;
        chordView.open();
    }

    visibility: NOO.isAndroid() && GLOB.fullScreen() ? "FullScreen" : "AutomaticVisibility"
    visible: true
    modality: Qt.WindowModal
    title: qsTr("Import of musical score")
    width: nootkaWindow.width
    height: nootkaWindow.height
    x: nootkaWindow.x
    y: nootkaWindow.y
    color: activPal.window
    Component.onCompleted: {
        melImport.importWindowReady();
        if (GLOB.gotIt("ScoreImport", true))
            Qt.createComponent("qrc:/gotit/ImportInfo.qml").createObject(importWindow, {
            "remaindChecked": true
        });

        if (melImport.partsModel.length === 0)
            busyComp.createObject(melImport);

        partList.model = Qt.binding(function() {
            return melImport.partsModel;
        });
    }
    onClosing: destroy()

    ButtonGroup {
        id: group
    }

    TmelodyImportItem {
        id: melImport

        anchors.fill: parent

        Text {
            id: melTitle

            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            text: melImport.title

            font {
                pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2)
                bold: true
            }

        }

        ListView {
            id: partList

            y: melTitle.height + NOO.factor() * (NOO.isAndroid() ? 1.1 : 2)
            width: parent.width
            height: parent.height - melTitle.height - buttRow.height - NOO.factor() * 2.5
            clip: true
            spacing: NOO.factor() / (NOO.isAndroid() ? 4 : 2)

            //model: // apply model only when entire window is created
            delegate: TipRect {
                x: NOO.factor() / 4
                width: melImport.width - NOO.factor() / 2
                height: scoreCol.height + NOO.factor()

                Text {
                    z: 5
                    x: NOO.factor() * 2
                    color: activPal.text
                    text: modelData.partName
                }

                Column {
                    id: scoreCol

                    width: parent.width
                    z: 1

                    Repeater {
                        model: modelData.snippets

                        Rectangle {
                            width: parent.width
                            height: score.height + NOO.factor()
                            color: NOO.alpha(activPal.highlight, importChB.checked ? 50 : 0)
                            Component.onCompleted: {
                                modelData.setScoreObject(score.scoreObj);
                                modelData.selected = Qt.binding(function() {
                                    return importChB.checked;
                                });
                                if (modelData.unsupported)
                                    unsuppComp.createObject(this);

                            }

                            TcheckBox {
                                id: importChB

                                enabled: modelData.unsupported === 0
                                anchors.verticalCenter: parent.verticalCenter
                                ButtonGroup.group: multiSelect ? null : group
                            }

                            Score {
                                id: score

                                y: NOO.factor() / 2
                                x: importChB.width + NOO.factor()
                                width: melImport.width - importChB.width - NOO.factor() * 1.5
                                height: NOO.isAndroid() ? Math.min(NOO.factor() * 8, importWindow.height / 3) : NOO.factor() * 15
                                readOnly: true
                                bgColor: "transparent"
                            }

                        }

                    }

                }

                Rectangle {
                    z: 7
                    width: childrenRect.width
                    height: childrenRect.height
                    radius: height / 4
                    color: NOO.alpha(activPal.base, 200)

                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: NOO.factor() / 2
                    }

                    // single part actions for 'Divide' & 'Transpose'
                    Row {
                        RectButton {
                            height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
                            text: "\u2702"
                            onClicked: {
                                if (!dividePop)
                                    dividePop = divideComp.createObject(importWindow);

                                var p = parent.mapToItem(partList, 0, (NOO.factor() * 15 - dividePop.height) / 2);
                                dividePop.melPart = modelData;
                                dividePop.x = partList.width - dividePop.width - NOO.factor() * 4;
                                dividePop.y = NOO.bound(0, p.y, partList.height - dividePop.height);
                                dividePop.open();
                            }

                            font {
                                pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2)
                                family: "Nootka"
                            }

                        }

                        RectButton {
                            height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
                            text: "\u0192"
                            onClicked: {
                                if (!transPop)
                                    transPop = transComp.createObject(importWindow);

                                var p = parent.mapToItem(partList, 0, 0);
                                transPop.melPart = modelData;
                                transPop.initialKey = modelData.key;
                                transPop.x = partList.width - transPop.width - NOO.factor() * 4;
                                transPop.y = p.y;
                                transPop.open();
                            }

                            font {
                                pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2)
                                family: "Nootka"
                            }

                        }

                        RectButton {
                            height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
                            text: "\u0193"
                            onClicked: {
                                if (!allChordsPop)
                                    allChordsPop = allChordsComp.createObject(importWindow);

                                var p = parent.mapToItem(partList, 0, 0);
                                allChordsPop.melPart = modelData;
                                allChordsPop.x = partList.width - allChordsPop.width - NOO.factor() * 4;
                                allChordsPop.y = p.y;
                                allChordsPop.open();
                            }

                            font {
                                pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2)
                                family: "Nootka"
                            }

                        }

                    }

                }

            }

            ScrollBar.vertical: ScrollBar {
            }

        }

        // Footer with 'Import' & 'Cancel' buttons
        Row {
            id: buttRow

            y: parent.height - height - NOO.factor() / 2
            x: (parent.width - width) / 2
            spacing: NOO.factor() * 2

            TiconButton {
                y: NOO.factor() / 4
                pixmap: NOO.pix("exit")
                text: NOO.TR("QShortcut", "Cancel")
                color: Qt.tint(activPal.button, NOO.alpha("red", NOO.isAndroid() ? 40 : 0))
                onClicked: close()
            }

            TiconButton {
                y: NOO.factor() / 4
                pixmap: NOO.pix("melody")
                text: qsTr("Import")
                color: Qt.tint(activPal.button, NOO.alpha("teal", NOO.isAndroid() ? 40 : 0))
                onClicked: {
                    melImport.emitImport();
                    close();
                }
            }

        }

        // Global 'Divide' action
        Row {
            anchors {
                left: parent.left
                top: parent.top
                topMargin: NOO.factor() / 4
                leftMargin: NOO.factor() / 2
            }

            RectButton {
                height: NOO.factor() * (NOO.isAndroid() ? 2 : 3)
                text: "\u2702"
                onClicked: {
                    if (!dividePop)
                        dividePop = divideComp.createObject(importWindow);

                    dividePop.melPart = null;
                    dividePop.x = (importWindow.width - dividePop.width) / 2;
                    dividePop.y = NOO.factor() * 2;
                    dividePop.open();
                }

                font {
                    pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2.8)
                    family: "Nootka"
                }

            }

            RectButton {
                height: NOO.factor() * (NOO.isAndroid() ? 2 : 3)
                text: "\u0193"
                onClicked: {
                    if (!allChordsPop)
                        allChordsPop = allChordsComp.createObject(importWindow);

                    allChordsPop.melPart = null;
                    allChordsPop.x = (importWindow.width - allChordsPop.width) / 2;
                    allChordsPop.y = NOO.factor() * 2;
                    allChordsPop.open();
                }

                font {
                    pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2.8)
                    family: "Nootka"
                }

            }

        }

    }

    Component {
        id: divideComp

        TpopupDialog {
            property var melPart: null

            width: divMel.width + NOO.factor() * 2
            height: divMel.height + NOO.factor() * 7
            caption: melPart ? "" : qsTr("Transform all parts of the score")
            onAccepted: {
                if (melPart)
                    melPart.splitBarNr = divMel.divisionBy;
                else
                    melImport.globalSplitNr = divMel.divisionBy;
            }

            DivideMelody {
                id: divMel

                divisionBy: parent.melPart ? dividePop.melPart.splitBarNr : melImport.globalSplitNr
            }

        }

    }

    Component {
        id: transComp

        TpopupDialog {
            property var melPart: null
            property alias initialKey: transpose.initialKey

            width: transpose.width + NOO.factor() * 2
            height: transpose.height + NOO.factor() * (melPart ? 5 : 7)
            caption: melPart ? "" : qsTr("Transform all parts of the score")
            onAboutToShow: {
                transpose.toKey = false;
                transpose.byInterval = false;
            }
            onAccepted: {
                if (transpose.toKey || transpose.byInterval) {
                    if (transpose.toKey && melPart)
                        transPop.melPart.key = transpose.selectedKey;

                    melImport.transpose(transpose.outShift, transpose.outScaleToRest, transpose.inInstrumentScale, transPop.melPart);
                }
            }

            Transpose {
                id: transpose
            }

        }

    }

    Component {
        id: allChordsComp

        TpopupDialog {
            property var melPart: null

            width: chCol.width + NOO.factor() * 3
            height: chCol.height + NOO.factor() * (melPart ? 5 : 7)
            caption: melPart ? "" : qsTr("Transform all parts of the score")
            onAccepted: {
                if (melPart) {
                    if (chordNoteChB.checked)
                        melPart.selectNoteInChords(selChordNoteSpin.value, topBottCombo.currentIndex === 0);
                    else
                        melPart.arpeggiateChords();
                } else if (chordNoteChB.checked) {
                    melImport.selectNoteInChords(selChordNoteSpin.value, topBottCombo.currentIndex === 0);
                } else {
                    melImport.arpeggiateChords();
                }
            }

            Column {
                id: chCol

                spacing: NOO.factor()

                TcheckBox {
                    id: chordNoteChB

                    text: qsTr("in every chord")
                    onToggled: {
                        if (chordNoteChB.checked)
                            chordExplodeChB.checked = false;

                    }
                }

                Row {
                    spacing: NOO.factor()
                    x: NOO.factor()
                    enabled: chordNoteChB.checked

                    Text {
                        text: qsTr("select", "[1st, 2nd, ...] note")
                        color: activPal.text
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TspinBox {
                        id: selChordNoteSpin

                        from: 1
                        to: 10
                    }

                    Text {
                        text: qsTr("note", "select [1st, 2nd, ...] note [from the top/bottom]")
                        color: activPal.text
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TcomboBox {
                        id: topBottCombo

                        width: NOO.factor() * 12
                        anchors.verticalCenter: parent.verticalCenter
                        model: [qsTr("from the top"), qsTr("from the bottom")]
                    }

                }

                TcheckBox {
                    id: chordExplodeChB

                    text: qsTr("arpeggiate chords and insert all notes")
                    onToggled: {
                        if (chordExplodeChB.checked)
                            chordNoteChB.checked = false;

                    }
                }

            }

        }

    }

    Component {
        id: chordComp

        // preview of chord notes
        MelodyPreview {
            property var chordIt: null

            melody: chordIt ? chordIt.chord : null
            showButtons: false
            width: NOO.factor() * 24
            height: NOO.factor() * 26
            caption: chordIt && chordIt.selectSingle ? qsTr("Select one of the notes") : qsTr("Arpeggiate chord")
            acceptButton.visible: chordIt && !chordIt.selectSingle
            selectReadOnly: true
            onReadOnlyNoteClicked: {
                hi.parent = score.note(noteId);
                chordIt.selected = noteId;
            }
            onAboutToShow: {
                reload();
                hi.parent = score.note(chordIt.selected);
                var p = parent.mapFromItem(chordIt, 0, chordIt.height / 2);
                x = p.x - (p.x > parent.width / 2 ? width + NOO.factor() : -NOO.factor() * 2);
                y = NOO.bound(NOO.factor(), p.y - height / 2, parent.height - height - NOO.factor());
            }
            onAccepted: chordIt.arpeggiateChord()

            // selected chord note highlight
            Rectangle {
                id: hi

                parent: null
                visible: parent != null
                width: parent ? (parent.width - parent.alterWidth) * 1.5 : 0
                height: parent ? Math.min(12, parent.notePosY + 6) : 0
                x: parent ? -width * 0.25 : 0
                y: parent ? Math.min(parent.height - height, Math.max(0, parent.notePosY - height / 2)) : 0
                color: NOO.alpha(activPal.highlight, 75)
                z: -1
                radius: width / 3
            }

            RectButton {
                height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
                visible: chordIt && chordIt.selectSingle
                text: "\u0194"
                onClicked: {
                    chordIt.setRhythm();
                    reload();
                }

                anchors {
                    left: parent.left
                    top: parent.top
                }

                font {
                    pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2)
                    family: "Nootka"
                }

                ToolTip {
                    delay: 150
                    timeout: 5000
                    x: parent.width + NOO.factor()
                    y: (parent.height - height) / 2
                    visible: parent.hovered

                    contentItem: Text {
                        text: qsTr("Arpeggiate chord")
                        color: activPal.text
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

                    background: TipRect {
                        shadowRadius: NOO.factor()
                        color: activPal.base
                    }

                }

            }

        }

    }

    Component {
        id: unsuppComp

        Text {
            x: NOO.factor()
            y: NOO.factor() * 1.2
            color: activPal.text
            style: Text.Sunken
            styleColor: "red"
            text: qsTr("This fragment contains elements of musical score which are not supported by Nootka!")
        }

    }

    Component {
        id: busyComp

        BusyIndicator {
            anchors.centerIn: parent
            scale: Math.max(1, (parent.width / 10) / width)
            running: melImport.partsModel.length === 0
        }

    }

}
