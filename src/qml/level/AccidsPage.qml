/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tflickable {
    height: parent.height
    contentHeight: accidsGrid.height + NOO.factor() * 2
    contentWidth: Math.max(width, NOO.factor() * 40)

    Grid {
        id: accidsGrid

        columns: parent.width > NOO.factor() * 60 ? 2 : 1
        width: parent.width
        spacing: NOO.factor() / 4
        horizontalItemAlignment: Grid.AlignHCenter
        topPadding: NOO.factor()

        Tframe {
            width: accidsGrid.columns === 1 ? Math.max(parent.width * 0.9, dblAccidsChB.width) : parent.width * 0.4
            enabled: creator.howGetMelody !== 2 || !creator.isMelody

            Column {
                spacing: NOO.factor() / 2
                width: parent.width

                Text {
                    text: qsTr("accidentals")
                    color: activPal.text
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Tile {
                    description: qsTr("Sharps will be used in questions and answers. It has to be checked, if keys with sharps are used.")

                    TcheckBox {
                        id: sharpsChB

                        enabled: !(useKeysChB.checked && (loKeyCombo.currentIndex > 7 || (rangeKeysChB.checked && hiKeyCombo.currentIndex > 7)))
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("# - sharps")
                        checked: creator.withSharps
                        onClicked: creator.withSharps = checked
                    }

                }

                Tile {
                    description: qsTr("Flats will be used in questions and answers. It has to be checked, if keys with flats are used.")

                    TcheckBox {
                        id: flatsChB

                        enabled: !(useKeysChB.checked && (loKeyCombo.currentIndex < 7 || (rangeKeysChB.checked && hiKeyCombo.currentIndex < 7)))
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("b - flats")
                        checked: creator.withFlats
                        onClicked: creator.withFlats = checked
                    }

                }

                Tile {
                    TcheckBox {
                        id: dblAccidsChB

                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("x, bb - double accidentals")
                        checked: creator.withDblAccids
                        onClicked: creator.withDblAccids = checked
                    }

                }

                Tile {
                    description: qsTr("if checked, it is possible to select a note with given accidental only.")

                    TcheckBox {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("force using appropriate accidental")
                        checked: creator.forceAccids
                        onClicked: creator.forceAccids = checked
                    }

                }

            }
            // accids frame

        }

        Tframe {
            id: keyFrame

            width: accidsGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.59

            Column {
                spacing: NOO.factor() / 2
                width: parent.width

                TcheckBox {
                    id: useKeysChB

                    text: NOO.TR("ScorePage", "use key signatures")
                    anchors.horizontalCenter: parent.horizontalCenter
                    checked: creator.useKeySign
                    onClicked: creator.useKeySign = checked
                }

                ButtonGroup {
                    id: keysGr
                }

                Tile {
                    enabled: useKeysChB.checked
                    description: qsTr("Only one, selected key signature for whole exam.")

                    TradioButton {
                        text: qsTr("single key")
                        ButtonGroup.group: keysGr
                        anchors.horizontalCenter: parent.horizontalCenter
                        checked: creator.isSingleKey
                        onClicked: creator.isSingleKey = checked
                    }

                }

                Tile {
                    enabled: useKeysChB.checked
                    description: qsTr("Random key signature from selected range.")

                    TradioButton {
                        id: rangeKeysChB

                        text: qsTr("range of keys")
                        ButtonGroup.group: keysGr
                        anchors.horizontalCenter: parent.horizontalCenter
                        checked: !creator.isSingleKey
                        onClicked: creator.isSingleKey = !checked
                    }

                }

                Tile {
                    enabled: useKeysChB.checked
                    description: qsTr("Select a key signature. Appropriate accidentals used in this level will be selected automatically.")

                    Row {
                        spacing: NOO.factor() / 2
                        anchors.horizontalCenter: parent.horizontalCenter

                        TcomboBox {
                            id: loKeyCombo

                            model: NOO.keyComboModel()
                            width: keyFrame.width * 0.4
                            currentIndex: creator.loKey + 7
                            onActivated: creator.loKey = currentIndex - 7
                        }

                        Rectangle {
                            color: enabled ? activPal.text : disdPal.text
                            width: NOO.factor()
                            height: NOO.factor() / 5
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        TcomboBox {
                            id: hiKeyCombo

                            enabled: rangeKeysChB.checked
                            model: NOO.keyComboModel()
                            width: keyFrame.width * 0.4
                            currentIndex: creator.hiKey + 7
                            onActivated: creator.hiKey = currentIndex - 7
                        }

                    }

                }

                Tile {
                    enabled: useKeysChB.checked
                    description: qsTr("if checked, in exam user have to select a key signature, otherwise it is shown by application.")

                    TcheckBox {
                        text: qsTr("select a key signature manually")
                        anchors.horizontalCenter: parent.horizontalCenter
                        checked: creator.manualKey
                        onClicked: creator.manualKey = checked
                    }

                }

                CurrentKeyTile {
                    enabled: creator.howGetMelody !== 2 || !creator.isMelody
                    checked: creator.onlyCurrKey
                    checkBox.onClicked: creator.onlyCurrKey = checked
                    description: qsTr("Only notes from current key signature are taken. If key signature is disabled accidentals are not used.")
                }

            }
            // keyFrame

        }

    }

}
