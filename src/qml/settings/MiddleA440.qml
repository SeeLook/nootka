/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tile {
    property alias value: freqSpin.value
    property bool showDesc: true

    description: !showDesc ? "" : qsTranslate("SoundPage", "The base frequency of <i>middle a</i>.<br>Detection of the proper pitch of notes is relative to this value. This also affects the pitch of played sounds.")

    Column {
        width: parent.width

        Row {
            spacing: NOO.factor()
            anchors.horizontalCenter: parent.horizontalCenter

            TlabelText {
                text: qsTranslate("SoundPage", "middle A")
            }
            // staff

            Text {
                height: NOO.factor() * 4.5
                y: NOO.factor() * -1.6
                text: "\ue014\ue014\ue014\ue014"
                color: enabled ? activPal.text : disdPal.text

                font {
                    pixelSize: NOO.factor() * 2.5
                    family: "scorek"
                }

                // clef
                Text {
                    x: NOO.factor() / 4
                    y: NOO.factor() * -0.6
                    color: parent.color
                    font: parent.font
                    text: "\ue050"
                }

                // note
                Text {
                    color: parent.color
                    font: parent.font
                    x: NOO.factor() * 2.7
                    y: NOO.factor() * -0.9
                    text: "\ue1d7"
                }

            }

            TspinBox {
                id: freqSpin

                anchors.verticalCenter: parent.verticalCenter
                from: 391 // in range of two semitones up and down around middle A (440Hz)
                to: 493
            }

            TlabelText {
                text: qsTranslate("SoundPage", "[Hz]")
            }

        }

    }

}
