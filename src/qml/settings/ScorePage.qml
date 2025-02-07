/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import Score 1.0

Column {
    function save() {
        var prevSingleMode = GLOB.singleNoteMode;
        GLOB.singleNoteMode = singleNoteModeChB.checked;
        if (singleNoteModeChB.checked) {
            GLOB.showEnharmNotes = showEnharmNotesChB.checked;
            GLOB.enharmNoteColor = enharmNoteColor.color;
        } else if (prevSingleMode != GLOB.singleNoteMode) {
            NOO.mainScore.scoreObj.editMode = true;
        }
        GLOB.rhythmsEnabled = rhythmsEnabledChB.checked;
        GLOB.enableDoubleAccids = doubleAccidsChB.checked;
        GLOB.noteCursorColor = pointerColorButt.color;
        GLOB.keySignatureEnabled = enableKeyChB.checked;
        GLOB.scientificOctaves = scientificRadio.checked;
        GLOB.noteNameStyle = nameStyleSel.style;
        GLOB.seventhIsB = is7BSelector.is7B;
        GLOB.namesOnScore = namesOnScoreChB.checked;
        GLOB.nameColor = nameColorButt.color;
        // GLOB.clefType saved in TsettingsDialog.qml
        if (GLOB.keySignatureEnabled) {
            GLOB.showKeyName = showKeyNamesChB.checked;
            if (GLOB.showKeyName) {
                GLOB.majorKeyNameSufix = majKeySufixText.sufix;
                GLOB.minorKeyNameSufix = minKeySufixText.sufix;
                GLOB.keyNameStyle = keyNameStyleSel.style;
                GLOB.updateKeySignatureNames();
            }
        }
    }

    function defaults() {
        singleNoteModeChB.checked = false;
        showEnharmNotesChB.checked = false;
        enharmNoteColor.color = Qt.rgba(0, 0.635294, 0.635294, 1);
        doubleAccidsChB.checked = false;
        pointerColorButt.color = "pink";
        settings.clef = GLOB.instrument.clef;
        enableKeyChB.checked = false;
        musicalRadio.checked = true;
    }

    function help() {
        NOO.openDocLink("2017/05/17/score-settings/");
    }

    width: parent.width
    height: parent.height

    TbuttonBar {
        id: headList

        model: [qsTr("Score settings"), qsTr("Key signatures"), qsTr("Clefs"), qsTr("Notes naming")]
        onCurrentIndexChanged: swipePages.currentIndex = currentIndex
    }

    StackLayout {
        id: swipePages

        height: parent.height - headList.height
        width: parent.width

        // 1st page (general)
        Tflickable {
            height: parent.height
            contentHeight: firstColumn.height + NOO.factor() * 2
            contentWidth: Math.max(width, NOO.factor() * 35)
            Component.onCompleted: enharmNoteColor.color = GLOB.enharmNoteColor

            Column {
                id: firstColumn

                width: parent.width
                spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2

                Tframe {
                    width: parent.width * 0.99
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2
                        width: parent.width

                        Tile {
                            description: qsTr("When enabled, a score displays only a single note.")

                            TcheckBox {
                                id: singleNoteModeChB

                                text: qsTr("use single note only")
                                checked: GLOB.singleNoteMode
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                        }

                        Tile {
                            enabled: singleNoteModeChB.checked
                            description: qsTr("Shows enharmonic variants of notes.<br>i.e.: the note E is also Fb (F flat) <i>and</i> Dx (D with double sharp).")

                            TcheckBox {
                                id: showEnharmNotesChB

                                checked: GLOB.showEnharmNotes
                                text: qsTr("show enharmonic variants of notes")
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                        }

                        Tile {
                            enabled: singleNoteModeChB.checked

                            Row {
                                spacing: NOO.factor()
                                anchors.horizontalCenter: parent.horizontalCenter

                                TlabelText {
                                    text: qsTr("color of enharmonic notes")
                                }

                                ColorButton {
                                    id: enharmNoteColor

                                    title: qsTr("color of enharmonic notes")
                                }

                            }

                        }

                    }

                }

                Tile {
                    description: qsTr("Use rhythms or only bare note heads.")
                    enabled: !singleNoteModeChB.checked

                    TcheckBox {
                        id: rhythmsEnabledChB

                        text: qsTr("enable rhythms")
                        anchors.horizontalCenter: parent.horizontalCenter
                        checked: GLOB.rhythmsEnabled
                    }

                }

                Tile {
                    description: qsTr("If checked, you can use double sharps and double flats.")

                    TcheckBox {
                        id: doubleAccidsChB

                        text: qsTr("use double accidentals")
                        anchors.horizontalCenter: parent.horizontalCenter
                        checked: GLOB.enableDoubleAccids
                    }

                }

                Tile {
                    Row {
                        spacing: NOO.factor()
                        anchors.horizontalCenter: parent.horizontalCenter

                        TlabelText {
                            text: qsTr("note-cursor color")
                        }

                        ColorButton {
                            id: pointerColorButt

                            color: GLOB.noteCursorColor
                            title: qsTr("note-cursor color")
                        }

                    }

                }

            }

        }

        // 2nd page (key signatures)
        Tflickable {
            height: parent.height
            contentHeight: secondColumn.height + NOO.factor() * 2
            contentWidth: Math.max(width, NOO.factor() * 35)

            Column {
                id: secondColumn

                width: parent.width
                spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2

                TcheckBox {
                    id: enableKeyChB

                    text: qsTr("use key signatures")
                    anchors.horizontalCenter: parent.horizontalCenter
                    checked: GLOB.keySignatureEnabled
                }

                Tframe {
                    width: parent.width * 0.99
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2
                        width: parent.width

                        Tile {
                            enabled: enableKeyChB.checked

                            TcheckBox {
                                id: showKeyNamesChB

                                text: qsTr("show names of key signature")
                                anchors.horizontalCenter: parent.horizontalCenter
                                checked: GLOB.showKeyName
                            }

                        }

                        Tile {
                            enabled: enableKeyChB.checked && showKeyNamesChB.checked
                            Component.onCompleted: {
                                majKeySufixText.sufix = GLOB.majorKeyNameSufix;
                                minKeySufixText.sufix = GLOB.minorKeyNameSufix;
                                keyNameStyleSel.style = GLOB.keyNameStyle;
                            }

                            Grid {
                                columns: parent.width < NOO.factor() * 50 ? 1 : 2
                                spacing: NOO.factor()
                                anchors.horizontalCenter: parent.horizontalCenter
                                horizontalItemAlignment: Grid.AlignHCenter

                                NameStyleSelector {
                                    id: keyNameStyleSel

                                    seventhIsB: is7BSelector.is7B
                                }

                                Column {
                                    Text {
                                        text: qsTr("Naming extension")
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        color: enabled ? activPal.text : disdPal.text
                                    }

                                    Row {
                                        spacing: NOO.factor() * 2
                                        anchors.horizontalCenter: parent.horizontalCenter

                                        KeySufixEdit {
                                            id: majKeySufixText

                                            nameStyle: keyNameStyleSel.style
                                        }

                                        KeySufixEdit {
                                            id: minKeySufixText

                                            nameStyle: keyNameStyleSel.style
                                            noteOne: 1
                                            alterOne: 1
                                            noteTwo: 5
                                            alterTwo: 0
                                        }

                                    }

                                }

                            }

                        }

                    }

                }

            }

        }

        // 3rd page (clefs)
        Column {
            width: parent.width
            spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2

            Text {
                id: clefText

                text: qsTr("Select default clef for the application.") + "<br><b>" + qsTr("Remember! Not all clefs are suitable for some possible tunings or instrument types!") + "<b>"
                textFormat: Text.StyledText
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
                wrapMode: Text.WordWrap
                color: activPal.text
            }

            ClefMenu {
                id: clefs

                width: parent.width
                height: swipePages.height - parent.spacing - clefText.height
                selClef: settings.clef
                onSelClefChanged: settings.clef = selClef
            }

        }

        // 4rd page (note name calling)
        Tflickable {
            contentHeight: nameCol.height + NOO.factor() * 2
            contentWidth: Math.max(width, NOO.factor() * 35)

            Column {
                id: nameCol

                width: parent.width
                spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2
                anchors.horizontalCenter: parent.horizontalCenter
                Component.onCompleted: {
                    nameStyleSel.style = GLOB.noteNameStyle;
                    is7BSelector.is7B = GLOB.seventhIsB;
                }

                Tile {
                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: NOO.factor()

                        TcheckBox {
                            id: namesOnScoreChB

                            text: qsTr("Show names of all notes on the score")
                            anchors.horizontalCenter: parent.horizontalCenter
                            checked: GLOB.namesOnScore
                        }

                        Row {
                            spacing: NOO.factor()
                            enabled: namesOnScoreChB.checked
                            anchors.horizontalCenter: parent.horizontalCenter

                            TlabelText {
                                text: qsTr("names highlight color")
                            }

                            ColorButton {
                                id: nameColorButt

                                color: GLOB.nameColor
                                title: qsTr("names highlight color")
                            }

                        }

                    }

                }

                Tile {
                    description: qsTr("Naming style of note. The main difference is the 7th note.<br>Is it B and B flat, or H and B?")

                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: NOO.factor() * 2

                        Select7note {
                            id: is7BSelector

                            anchors.horizontalCenter: parent.horizontalCenter
                            style: nameStyleSel.style
                        }

                        NameStyleSelector {
                            id: nameStyleSel

                            seventhIsB: is7BSelector.is7B
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                    }

                }

                Tile {
                    description: qsTr("Scientific (international) pitch notation is widely used in technical sources and tuning devices/applications, when the other notation style is used more in music publications.")

                    ButtonGroup {
                        id: octaveGr
                    }

                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width * 0.9
                        spacing: NOO.factor()

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: NOO.factor() * 2

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("Octave numbers")
                                color: activPal.text
                            }

                            TradioButton {
                                id: scientificRadio

                                text: qsTr("scientific")
                                ButtonGroup.group: octaveGr
                                checked: GLOB.scientificOctaves
                            }

                            TradioButton {
                                id: musicalRadio

                                text: qsTr("musical")
                                ButtonGroup.group: octaveGr
                                checked: !GLOB.scientificOctaves
                            }

                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: activPal.text
                            textFormat: Text.RichText
                            text: "<table>" + "<tr><td> <b>" + NOO.TR("TnameItem", "Octaves") + ":</b> </td><td> <b>" + qsTr("scientific") + "</b> </td><td> <b>" + qsTr("musical") + "</b> </td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "Subcontra octave") + "</td><td align=\"center\">C<sub>0</sub></td><td align=\"center\">C<sub>2</sub></td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "Contra octave") + "</td><td align=\"center\">C<sub>1</sub></td><td align=\"center\">C<sub>1</sub></td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "Great octave") + "</td><td align=\"center\">C<sub>2</sub></td><td align=\"center\">C</td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "Small octave") + "</td><td align=\"center\">C<sub>3</sub></td><td align=\"center\">c</td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "One-line octave") + "</td><td align=\"center\">C<sub>4</sub></td><td align=\"center\">c<sup>1</sup></td></tr>" + "<tr><td>" + NOO.TR("TnoteName", "Two-line octave") + "</td><td align=\"center\">C<sub>5</sub></td><td align=\"center\">c<sup>2</sup></td></tr>" + "</table>"
                        }

                    }

                }

            }

        }

    }

}
