/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tflickable {
    id: instrPage

    property int instrument: -1

    function getInstrument() {
        return instrSel.instrument;
    }

    height: parent.height
    contentHeight: instrCol.height
    contentWidth: width

    Tile {
        y: (instrPage.height - height) / 2
        bgColor: Qt.tint(nootkaWindow.labelColor, NOO.alpha(activPal.base, 230))

        bgBorder {
            width: 2
            color: nootkaWindow.labelColor
        }

        Column {
            id: instrCol

            width: parent.width
            spacing: NOO.factor() * 3

            Text {
                text: qsTr("What instrument do you play?")
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: activPal.text

                font {
                    pixelSize: NOO.factor() * 2
                    bold: true
                }

            }

            InstrumentSelector {
                id: instrSel

                height: nootkaWindow.width / 7
                anchors.horizontalCenter: parent.horizontalCenter
                instrument: GLOB.instrument.type
            }

        }

    }

    Connections {
        target: nootkaWindow.swipe
        onCurrentIndexChanged: {
            if (nootkaWindow.swipe.currentIndex != 1 && instrPage.instrument != instrSel.instrument)
                instrPage.instrument = instrSel.instrument;

        }
    }

}
