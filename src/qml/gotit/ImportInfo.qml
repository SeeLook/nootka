/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Main 1.0
import QtQuick 2.12

GotIt {
    id: gotImport

    // private
    property real textTop: gotImport.height - Math.min(impCol.height + NOO.factor() * (NOO.isAndroid() ? 3 : 9), gotImport.height * 0.7)

    gotItType: TgotIt.GotImportScore
    visible: true
    onClicked: {
        if (showGotIt) {
            // store only if user doesn't want it next time

            // Got It, not help dialog
            if (!remaindChecked)
                GLOB.setGotIt("ScoreImport", false);

        }
        close();
        destroy();
    }

    Tflickable {
        width: parent.width
        height: parent.height * 0.7
        y: textTop
        contentHeight: impCol.height + NOO.factor() * 4
        contentWidth: width

        Column {
            id: impCol

            spacing: NOO.factor()
            leftPadding: NOO.factor()
            width: parent.width - NOO.factor() * 13

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Unfortunately Nootka cannot display all notes of this score at once.")
                color: activPal.text

                font {
                    bold: true
                    pixelSize: NOO.factor() * 1.5
                }

            }

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                text: qsTr("But you can select here some voice or a part and import it to the application.") + "<br>" + qsTr("Before that, you can transform (split, transpose and etc.) score parts and also select which note of a chord will be imported.") + "<br>" + qsTr("Voices are divided automatically when meter, clef or key changes.")
                color: activPal.text
                font.pixelSize: NOO.factor() * 1.1
            }

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                text: qsTr("When you are preparing an exam or an exercise from the score parts, more fragments can be imported at once.")
                color: activPal.text
                font.pixelSize: NOO.factor() * 1.1
            }

        }

    }

    background: Rectangle {

        gradient: Gradient {
            GradientStop {
                position: 0
                color: NOO.alpha(activPal.base, 150)
            }

            GradientStop {
                position: textTop / gotImport.height - 0.1
                color: NOO.alpha(activPal.base, 150)
            }

            GradientStop {
                position: textTop / gotImport.height + 0.05
                color: Qt.tint(activPal.base, NOO.alpha("green", 50))
            }

        }

    }

}
