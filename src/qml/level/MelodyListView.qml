/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../score"
import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import Score 1.0

TmelodyListView {
    id: melListView

    property int currentMelody: -1
    property alias viewRoot: viewItem
    property var melPreview: null
    property var currWrapp: null
    property var dividePop: null
    property var transPop: null

    function showMelody(wrappId) {
        if (!melPreview) {
            melPreview = Qt.createComponent("qrc:/score/MelodyPreview.qml").createObject(NOO.isAndroid() ? nootkaWindow : melPage, {
                "width": (NOO.isAndroid() ? nootkaWindow.width : melListView.width) - NOO.factor() * 2,
                "maxHeight": melPage.height - NOO.factor() * (NOO.isAndroid() ? 4 : 10)
            });
            melPreview.wantDivide.connect(showDividePopup);
            melPreview.wantTranspose.connect(showTransposePopup);
        }
        currWrapp = wrappId;
        melPreview.open();
        melPreview.melody = getMelody(wrappId.nr);
        melPreview.idText = (wrappId.nr + 1) + ".";
        melPreview.border.color = wrappId.outOfScale ? "red" : activPal.highlight;
    }

    function removeWrapper(id) {
        melMod.remove(id);
        removeMelody(id);
        currentMelody = -1;
    }

    function moveMelody(from, to) {
        melMod.move(from, to, 1);
        swapMelodies(from, to);
    }

    function showDividePopup() {
        if (!dividePop)
            dividePop = divideComp.createObject(melPage);

        dividePop.open();
    }

    function showTransposePopup() {
        if (!transPop)
            transPop = transcomp.createObject(melPage);

        transPop.transpose.initialKey = currWrapp.wrapper.key;
        transPop.open();
    }

    implicitHeight: viewRow.height
    onAppendMelody: melMod.append({
    })
    onInsertMelody: melMod.insert(melId, {
    })
    onMelodiesChanged: creator.melodyListChanged()

    Row {
        id: viewRow

        Text {
            visible: !melView.visible
            y: NOO.factor() * 2
            width: melListView.width - NOO.factor() * 4
            color: activPal.text
            // TODO: Remove 2nd sentence from translations and here
            text: qsTr("Add here melodies from Music XML files.\nBut better keep them short, so divide them first in some external software.").split(".")[0] + "."
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            font {
                pixelSize: NOO.factor() * 1.2
                bold: true
            }

        }

        Item {
            id: viewItem

            visible: melView.count > 0
            width: melListView.width - NOO.factor() * 4 //melListView.height
            height: Math.max(buttCol.height, melPage.height)

            ListView {
                id: melView

                clip: true
                spacing: 1
                model: melMod

                anchors {
                    fill: parent
                    margins: 5
                }

                add: Transition {
                    enabled: GLOB.useAnimations

                    NumberAnimation {
                        property: "x"
                        from: -melListView.width
                        to: 0
                    }

                }

                remove: Transition {
                    enabled: GLOB.useAnimations

                    NumberAnimation {
                        property: "x"
                        to: -melListView.width
                    }

                }

                populate: Transition {
                    enabled: GLOB.useAnimations

                    NumberAnimation {
                        property: "x"
                        from: -melListView.width
                        to: 0
                    }

                }

                move: Transition {
                    enabled: GLOB.useAnimations

                    NumberAnimation {
                        property: "y"
                    }

                }

                moveDisplaced: Transition {
                    enabled: GLOB.useAnimations

                    NumberAnimation {
                        property: "y"
                    }

                }

                delegate: MelodyWrapper {
                    nr: index
                    width: melView.width - 10
                    Component.onCompleted: {
                        updateMelody();
                    }
                }

            }

        }

        Column {
            id: buttCol

            spacing: NOO.factor() * (NOO.isAndroid() ? 0.7 : 1)
            leftPadding: NOO.factor() / 2
            topPadding: NOO.factor() / (NOO.isAndroid() ? 2 : 1)

            TcuteButton {
                visible: NOO.isAndroid()
                width: NOO.factor() * 3
                text: "\u0191"
                textColor: enabled ? activPal.highlight : disdPal.text
                enabled: currentMelody > -1
                onClicked: showMelody(melView.currentItem)

                font {
                    pixelSize: NOO.factor() * 2.5
                    family: "Nootka"
                }

            }

            Item {
                width: 1
                height: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1.5)
            }

            TcuteButton {
                width: NOO.factor() * 3
                text: "+"
                textColor: "green"
                onClicked: {
                    loadMelody();
                    melView.positionViewAtEnd();
                }

                font {
                    pixelSize: NOO.factor() * 2.5
                    bold: true
                }

            }

            Item {
                width: 1
                height: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1.5)
            }

            TcuteButton {
                width: NOO.factor() * 3
                text: "\u2191"
                textColor: enabled ? "#008080" : disdPal.text
                enabled: currentMelody > 0
                onClicked: {
                    moveMelody(currentMelody, currentMelody - 1);
                    currentMelody--;
                }

                font {
                    pixelSize: NOO.factor() * 2.5
                    family: "Nootka"
                }

            }

            TcuteButton {
                width: NOO.factor() * 3
                text: "\u2193"
                textColor: enabled ? "#008080" : disdPal.text
                enabled: currentMelody > -1 && currentMelody < melMod.count - 1
                onClicked: {
                    moveMelody(currentMelody, currentMelody + 1);
                    currentMelody++;
                }

                font {
                    pixelSize: NOO.factor() * 2.5
                    family: "Nootka"
                }

            }

            Item {
                width: 1
                height: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1.5)
            }

            TcuteButton {
                width: NOO.factor() * 3
                text: "-"
                textColor: enabled ? "red" : disdPal.text
                enabled: currentMelody > -1
                onClicked: removeWrapper(currentMelody)

                font {
                    pixelSize: NOO.factor() * 2.5
                    bold: true
                }

                Shortcut {
                    enabled: currentMelody > -1
                    sequence: "del"
                    onActivated: removeWrapper(currentMelody)
                }

            }

        }

    }

    Component {
        id: divideComp

        TpopupDialog {
            width: divMel.width + NOO.factor() * 2
            height: divMel.height + NOO.factor() * 5
            onAccepted: {
                if (divMel.divisionBy > 0) {
                    melListView.divideMelody(currWrapp.nr, divMel.divisionBy);
                    currWrapp.reload();
                    if (melPreview) {
                        melPreview.reload();
                        melPreview.close();
                    }
                    creator.melodyListChanged();
                }
            }

            DivideMelody {
                id: divMel
            }

        }

    }

    Component {
        id: transcomp

        TpopupDialog {
            id: transPop

            property alias transpose: transpose

            width: transpose.width + NOO.factor() * 2
            height: transpose.height + NOO.factor() * 5
            onAboutToShow: transpose.currentKeyId = currWrapp ? currWrapp.wrapper.key + 7 : 7
            onAccepted: {
                if (transpose.toKey || transpose.byInterval) {
                    if (transpose.toKey)
                        currWrapp.wrapper.key = transpose.selectedKey;

                    melListView.transpose(transpose.outShift, transpose.outScaleToRest, transpose.inInstrumentScale, currWrapp.wrapper);
                    if (melPreview) {
                        melPreview.reload();
                        melPreview.close();
                    }
                    creator.melodyListChanged();
                }
            }

            Transpose {
                id: transpose
            }

        }

    }

    melodyModel: ListModel {
        id: melMod
    }

}
