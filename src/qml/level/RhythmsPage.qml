/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tflickable {
    //     Column {
    //       id: settCol
    //       anchors.verticalCenter: parent.verticalCenter
    //       TcheckBox {
    //         id: restsChB
    //         text: qsTr("rests")
    //         onClicked: restsClicked()
    //       }
    //       TcheckBox {
    //         id: tiesChB
    //         text: qsTr("ties")
    //         onClicked: tiesClicked()
    //       }
    //     }

    id: rtmPage

    contentHeight: rtmCol.height

    Column {
        id: rtmCol

        width: parent.width
        topPadding: NOO.factor() / 2
        spacing: NOO.factor() / 2
        enabled: creator.howGetMelody !== 2 && creator.melodyLen > 1

        Text {
            text: qsTr("Select any rhythm and time signature to be used in this level, otherwise melodies will not have rhythmic values, just bare note-heads.")
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - NOO.factor() * 2
            textFormat: Text.StyledText
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: enabled ? ((rtmSel.basicMask || rtmSel.dotsMask) && metersSel.meters ? activPal.text : "red") : disdPal.text

            font {
                pixelSize: NOO.factor()
                bold: true
            }

        }

        Row {
            width: parent.width
            spacing: NOO.factor() / 4

            Tile {
                id: selTile

                anchors.horizontalCenter: undefined
                width: rtmSel.width * 1.2
                height: Math.max(rtmPage.height - NOO.factor() * 2, rCol.height)

                RhythmSelector {
                    //           useRests: creator.useRests
                    //           onRestsClicked: creator.useRests = useRests
                    //           useTies: creator.useTies
                    //           onTiesClicked: creator.useTies = useTies

                    id: rtmSel

                    x: width / 10
                    height: selTile.height - NOO.factor() * 2
                    basicMask: creator.basicRhythms
                    dotsMask: creator.dotsRhythms
                    onBasicGroupChanged: creator.basicRhythms = basicMask
                    onDotsGroupChanged: creator.dotsRhythms = dotsMask
                }

            }

            Column {
                id: rCol

                width: parent.width - selTile.width - NOO.factor() / 4
                spacing: NOO.factor() / 4
                enabled: rtmSel.basicMask || rtmSel.dotsMask

                Tile {
                    description: qsTr("Time signatures to use in this level.")

                    MeterSelector {
                        id: metersSel

                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width - NOO.factor() * 2
                        meters: creator.meters
                        onMetersModified: creator.meters = meters
                    }

                }

                Tile {
                    description: qsTr("Number of measures in a melody. If 'variable' is set, it will grow up to the given value as an exercise or an exam is progressing.")

                    Row {
                        id: measRow

                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: NOO.factor()

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: NOO.TR("MelGenDialog", "Measures number")
                            color: enabled ? activPal.text : disdPal.text
                        }

                        TspinBox {
                            id: barNrSpin

                            from: 2
                            to: 32
                            value: creator.barNumber
                            onValueModified: creator.barNumber = value
                        }

                        TcheckBox {
                            text: qsTr("variable")
                            checked: creator.variableBarNr
                            onClicked: creator.variableBarNr = checked
                        }

                    }

                }

                RhythmDiversityTile {
                    diversity: creator.rhythmDiversity
                    onDiversityModified: creator.rhythmDiversity = diversity
                }

            }

        }

    }

}
