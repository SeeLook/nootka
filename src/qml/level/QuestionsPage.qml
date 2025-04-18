/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tflickable {
    id: qPage

    property real spacing: width / 100
    property real padding: width / 200
    // private
    property real maxWidth: 0

    height: parent.height
    contentHeight: questionCol.height + NOO.factor() * 2
    contentWidth: width

    Column {
        id: questionCol

        topPadding: qPage.padding
        width: parent.width

        Tframe {
            width: parent.width * 0.99
            anchors.horizontalCenter: parent.horizontalCenter
            bgColor: NOO.alpha(creator.isMelody ? NOO.invert(activPal.highlight) : activPal.highlight, 75)

            Column {
                width: parent.width

                Tile {
                    Row {
                        spacing: qPage.width / 50
                        anchors.horizontalCenter: parent.horizontalCenter

                        Text {
                            color: activPal.highlight
                            text: "n"
                            anchors.verticalCenter: parent.verticalCenter

                            font {
                                family: "Nootka"
                                pixelSize: NOO.factor() * 3
                            }

                        }

                        TradioButton {
                            text: qsTr("single note")
                            checked: !creator.isMelody
                            onClicked: creator.isMelody = !checked
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Rectangle {
                            color: activPal.dimText
                            width: parent.height / 12
                            height: parent.height * 0.9
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        TradioButton {
                            text: qsTr("melodies")
                            checked: creator.isMelody
                            onClicked: creator.isMelody = checked
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text {
                            color: NOO.invert(activPal.highlight)
                            text: "m"
                            anchors.verticalCenter: parent.verticalCenter

                            font {
                                family: "Nootka"
                                pixelSize: NOO.factor() * 3
                            }

                        }

                    }

                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    color: activPal.text
                    text: qsTr("Kind of questions and answers" + ":")
                }
                // single note

                Flow {
                    width: parent.width
                    spacing: qPage.spacing
                    padding: qPage.padding
                    visible: !creator.isMelody

                    Repeater {
                        id: questRep

                        model: 4

                        Item {
                            width: maxWidth
                            height: qBox.height

                            QuestionsBox {
                                id: qBox

                                qId: index
                                visible: index != 2 || GLOB.instrument.type !== 0
                                questionText: qsTr("Question") + " " + NOO.qaTypeText(index)
                                questionChecked: creator.questionAs & Math.pow(2, index)
                                answerBits: creator.answersAs[index]
                                Component.onCompleted: maxWidth = Math.max(maxWidth, qBox.width)
                            }

                        }

                    }

                }
                // melodies

                Grid {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: qPage.spacing
                    padding: qPage.padding
                    columns: playTile.width + writeTile.width + repeatTile.width + spacing * 3 < qPage.width ? 3 : (playTile.width + writeTile.width + spacing * 2 < qPage.width ? 2 : 1)
                    visible: creator.isMelody

                    Tile {
                        id: playTile

                        anchors.horizontalCenter: undefined
                        width: Math.max(playChB.width + NOO.factor() * 4, qPage.width * 0.3)
                        description: NOO.TR("Texam", "Play a melody written in a score")

                        TcheckBox {
                            id: playChB

                            anchors.horizontalCenter: parent.horizontalCenter
                            text: NOO.TR("Texam", "play melody")
                            checked: creator.playMelody
                            onClicked: creator.playMelody = checked
                        }

                    }

                    Tile {
                        id: writeTile

                        anchors.horizontalCenter: undefined
                        width: Math.max(writeChB.width + NOO.factor() * 4, qPage.width * 0.3)
                        description: NOO.TR("Texam", "Listen to a melody and write it on a score")

                        TcheckBox {
                            id: writeChB

                            anchors.horizontalCenter: parent.horizontalCenter
                            text: NOO.TR("Texam", "write melody")
                            checked: creator.writeMelody
                            onClicked: creator.writeMelody = checked
                        }

                    }

                    Tile {
                        id: repeatTile

                        visible: false // TODO in some next release
                        anchors.horizontalCenter: undefined
                        width: Math.max(repeatChB.width + NOO.factor() * 4, qPage.width * 0.3)
                        description: qsTranslate("Texam", "Listen to a melody and repeat it on your instrument")

                        TcheckBox {
                            id: repeatChB

                            enabled: false
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTranslate("Texam", "repeat melody")
                            checked: creator.repeatMelody
                            onClicked: creator.repeatMelody = checked
                        }

                    }

                }

            }

        }

        Flow {
            width: parent.width
            spacing: qPage.spacing
            padding: qPage.padding

            Tile {
                anchors.horizontalCenter: undefined
                width: Math.max(octaveChB.width + NOO.factor() * 2, qPage.width * 0.45)
                description: qsTr("if checked, selecting of valid octave is required")

                TcheckBox {
                    id: octaveChB

                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("require octave")
                    checked: creator.requireOctave
                    onClicked: creator.requireOctave = checked
                }

            }

            Tile {
                anchors.horizontalCenter: undefined
                width: Math.max(styleChB.width + NOO.factor() * 2, qPage.width * 0.45)
                description: qsTr("if checked, note names are switched between letters and solfege.")

                TcheckBox {
                    id: styleChB

                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("use different naming styles")
                    checked: creator.requireStyle
                    onClicked: creator.requireStyle = checked
                }

            }

            Tile {
                anchors.horizontalCenter: undefined
                visible: GLOB.instrument.isGuitar
                width: Math.max(strChB.width + NOO.factor() * 2, qPage.width * 0.45)
                description: qsTr("Shows on which string an answer has to be given. Be careful, sometimes it is needed and sometimes it makes no sense.")

                TcheckBox {
                    id: strChB

                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("show string number in questions")
                    checked: creator.showStrNr
                    onClicked: creator.showStrNr = checked
                }

            }

            Tile {
                anchors.horizontalCenter: undefined
                visible: GLOB.instrument.isGuitar
                width: Math.max(lowPosChB.width + NOO.factor() * 2, qPage.width * 0.45)
                description: qsTr("if checked, the lowest position in selected fret range is required, otherwise all possible positions of the note are acceptable. To use this, all strings have to be available!")

                TcheckBox {
                    id: lowPosChB

                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("notes in the lowest position only")
                    checked: creator.onlyLowPos
                    onClicked: creator.onlyLowPos = checked
                }

            }

        }

    }

}
