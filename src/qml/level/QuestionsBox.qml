/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tile {
    //   anchors.horizontalCenter: undefined

    property int qId: 0
    property alias questionText: questionChB.text
    property alias questionChecked: questionChB.checked
    property int answerBits: 0
    property var answSymb: ["s", "c", GLOB.instrument.glyph, "n"]

    width: col.width + NOO.factor() * 3
    description: unfold.checked ? qsTr("Select the type of answers for this kind of question.") : ""

    Column {
        id: col

        anchors.horizontalCenter: parent.horizontalCenter

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor() / 2

            TcheckBox {
                id: questionChB

                anchors.verticalCenter: parent.verticalCenter
                onClicked: creator.questionAs = checked ? creator.questionAs | Math.pow(2, qId) : creator.questionAs & ~Math.pow(2, qId)
            }

            Text {
                text: answSymb[qId] + "?"
                anchors.verticalCenter: parent.verticalCenter
                color: activPal.text

                font {
                    family: "nootka"
                    pixelSize: NOO.factor() * 2.5
                }

            }

            Text {
                id: unfold

                property bool checked: false

                anchors.verticalCenter: parent.verticalCenter
                color: ma.containsMouse ? activPal.highlight : activPal.text
                text: "   ⋮"

                font {
                    pixelSize: NOO.factor() * 2
                    bold: true
                }

                MouseArea {
                    id: ma

                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: unfold.checked = !unfold.checked
                }

            }

        }

        Row {
            spacing: NOO.factor() / 2
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: qsTr("answers") + ":"
                anchors.verticalCenter: parent.verticalCenter
                color: activPal.text
            }

            Grid {
                spacing: NOO.factor() / 2
                columns: unfold.checked ? 1 : 4

                Repeater {
                    model: 4

                    Row {
                        property alias checked: aChB.checked

                        visible: (index !== 2 || qId !== 2) || GLOB.instrument.isGuitar
                        layoutDirection: unfold.checked ? Qt.RightToLeft : Qt.LeftToRight
                        spacing: NOO.factor() / 2

                        TcheckBox {
                            id: aChB

                            text: unfold.checked ? NOO.qaTypeText(index) : ""
                            checked: answerBits & Math.pow(2, index)
                            onClicked: creator.setAnswers(qId, checked ? answerBits | Math.pow(2, index) : answerBits & ~Math.pow(2, index))
                        }

                        Text {
                            text: answSymb[index]
                            width: unfold.checked ? NOO.factor() * 3 : undefined
                            horizontalAlignment: Text.AlignHCenter
                            color: activPal.text

                            font {
                                family: "nootka"
                                pixelSize: NOO.factor() * 2.5
                            }

                        }

                    }

                }

            }

        }

    }

}
