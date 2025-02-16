/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick
import Nootka.Music
import Score 1.0

TpopupDialog {
    id: popup

    property alias melody: melPrev.melody
    property alias idText: nrText.text
    property alias showButtons: buttons.visible
    property alias selectReadOnly: melPrev.selectReadOnly
    property alias score: sc.scoreObj
    /**
       * If set score @p height has priority until it reaches @p maxHeight then score uses scrolling.
       * When unset, @p height property has to be set
       */
    property real maxHeight: 0

    signal wantDivide()
    signal wantTranspose()
    signal readOnlyNoteClicked(var noteId)

    function reload() {
        melPrev.reload();
    }

    height: maxHeight ? Math.min(maxHeight, header.height + sc.height + footer.height) : undefined
    bgColor: activPal.base
    acceptButton.visible: false
    rejectButton.text: NOO.TR("QShortcut", "Close")
    caption: melPrev.title

    border {
        color: activPal.highlight
        width: NOO.factor() / 4
    }

    TmelodyPreview {
        id: melPrev

        width: parent.width
        height: sc.height
        score: sc.scoreObj
        onReadOnlyNoteClicked: popup.readOnlyNoteClicked(noteId)

        Score {
            id: sc

            anchors.horizontalCenter: parent.horizontalCenter
            bgRect.color: "transparent"
            readOnly: true
            width: popup.width - NOO.factor()
            height: maxHeight ? Math.min(contentHeight, maxHeight - popup.footer.height) : popup.height - NOO.factor() * 4 - popup.header.height
            firstStaff.scale: ((NOO.factor() * (clef === Tclef.PianoStaffClefs ? 18 : 15)) / firstStaff.linesCount) * scaleFactor
            scoreObj.onStaffCreate: lastStaff.scale = firstStaff.scale
        }

    }

    Text {
        text: melPrev.composer
        x: parent.width - width - NOO.factor() / 2
        color: activPal.text

        font {
            bold: true
            pixelSize: NOO.factor() * 1.2
        }

    }

    Rectangle {
        id: buttons

        x: NOO.factor() / 2
        color: NOO.alpha(activPal.base, 200)
        width: childrenRect.width
        height: childrenRect.height
        radius: NOO.factor() / 2

        Row {
            z: 7
            spacing: NOO.factor() / 2

            Text {
                id: nrText

                color: activPal.text

                font {
                    bold: true
                    pixelSize: NOO.factor() * 2
                }

            }

            RectButton {
                height: NOO.factor() * 2.2
                text: "\u2702"
                onClicked: wantDivide()

                font {
                    pixelSize: NOO.factor() * 2
                    family: "Nootka"
                }

            }

            RectButton {
                height: NOO.factor() * 2.2
                text: "\u0192"
                onClicked: wantTranspose()

                font {
                    pixelSize: NOO.factor() * 2
                    family: "Nootka"
                }

            }

        }

    }

}
