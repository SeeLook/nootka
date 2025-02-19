/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../settings"
import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick
import Nootka.Music
import QtQuick.Controls 2.12

TtunerDialogItem {
    id: tunerDialog

    property bool widthEnough: dialLoader.width > dialLoader.height

    width: parent.width
    height: parent.height
    Component.onCompleted: {
        dialLoader.stdButtons = DialogButtonBox.Close;
        dialLoader.title = qsTr("Nooter - Nootka tuner");
    }

    Drawer {
        id: exDrawer

        width: exCol.width + NOO.factor() * 2
        height: parent.height
        visible: widthEnough
        modal: false
        closePolicy: widthEnough ? Popup.NoAutoClose : Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Column {
            id: exCol

            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor() * 1.5

            Item {
                width: NOO.factor()
                height: NOO.factor()
            }

            Repeater {
                model: tuningModel

                Row {
                    property var splitText: modelData.split(";")

                    Text {
                        visible: GLOB.instrument.isGuitar
                        anchors.verticalCenter: parent.verticalCenter
                        color: activPal.text
                        textFormat: Text.Normal
                        text: index + 1

                        font {
                            family: "Nootka"
                            pixelSize: NOO.factor() * 2.3
                        }

                    }

                    Item {
                        width: childrenRect.width
                        height: NOO.factor() * 1.5
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            y: -height * 0.34
                            color: activPal.text
                            textFormat: Text.StyledText
                            text: splitText[0]

                            font {
                                family: "Scorek"
                                pixelSize: NOO.factor() * 2
                            }

                        }

                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: activPal.text
                        textFormat: Text.Normal
                        text: "="

                        font {
                            family: "Nootka"
                            pixelSize: NOO.factor() * 1.2
                        }

                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: activPal.text
                        textFormat: Text.Normal
                        text: splitText[1]

                        font {
                            pixelSize: NOO.factor() * 1.4
                        }

                    }

                }

            }

            Text {
                width: parent.width
                color: activPal.text
                text: qsTranslate("SoundPage", "Frequency of detected note. You can use this for tuning.")
                font.pixelSize: NOO.factor() * 0.8
                wrapMode: Text.WordWrap
            }

        }

        background: TipRect {
            color: activPal.base
            radius: 0
        }

    }

    Column {
        y: spacing
        width: tunerDialog.width - (exDrawer.visible ? exDrawer.width : 0) - NOO.factor()
        x: exDrawer.position * exDrawer.width + NOO.factor() / 2
        spacing: (parent.height - freqTile.height - pitchCol.height) / 3

        MiddleA440 {
            id: freqTile

            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - NOO.factor()
            value: GLOB.midAfreq
            onValueChanged: workFreq = value
        }

        Column {
            id: pitchCol

            spacing: NOO.factor()
            width: parent.width

            Tumbler {
                id: pitchTumb

                width: parent.width
                height: NOO.factor() * 4
                anchors.horizontalCenter: parent.horizontalCenter
                visibleItemCount: ((width / (NOO.factor() * 5)) / 2) * 2 - 1
                model: highestNote() - lowestNote()
                currentIndex: pitch - lowestNote()
                wrap: false

                delegate: Component {
                    Column {
                        spacing: NOO.factor() / 4
                        opacity: 1 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
                        scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)

                        Item {
                            property int strNr: GLOB.instrument.isGuitar ? whichString(lowestNote() + modelData) : 0

                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                property real dev: Math.abs(deviation)

                                visible: parent.strNr > 0
                                x: strNameTxt.x + (strNameTxt.width - width) / 2
                                text: parent.strNr > 0 ? parent.strNr : ""
                                color: pitchTumb.currentIndex === index ? (dev > 0 ? (dev > 0.3 ? "red" : (dev > 0.1 ? "yellow" : "lime")) : activPal.text) : activPal.text
                                y: pitchTumb.currentIndex === index ? 0 : -height / 2
                                transformOrigin: Item.Top
                                scale: pitchTumb.currentIndex === index ? 1.6 : 1

                                font {
                                    family: "Nootka"
                                    pixelSize: NOO.factor() * 1.5
                                }

                                Behavior on color {
                                    ColorAnimation {
                                        duration: 150
                                    }

                                }

                            }

                            Text {
                                id: strNameTxt

                                visible: !GLOB.instrument.isGuitar || pitchTumb.currentIndex !== index
                                text: styledName(lowestNote() + modelData)
                                color: activPal.text
                                y: height * -0.2
                                x: -width / 2

                                font {
                                    family: "Scorek"
                                    pixelSize: NOO.factor() * 1.5
                                }

                            }

                        }

                    }

                }

                contentItem: PathView {
                    id: pathView

                    interactive: false
                    model: pitchTumb.model
                    delegate: pitchTumb.delegate
                    pathItemCount: pitchTumb.visibleItemCount + 1
                    preferredHighlightBegin: 0.5
                    preferredHighlightEnd: 0.5

                    path: Path {
                        startX: 0
                        startY: NOO.factor() * 1.4

                        PathLine {
                            x: pathView.width
                            y: NOO.factor() * 1.4
                        }

                    }

                }

            }

            IntonationBar {
                id: intoBar

                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - NOO.factor()
                height: NOO.factor() * 3
                deviation: tunerDialog.deviation
                pitchText: noteName
            }

            Row {
                id: freqTexts

                x: (parent.width - width) / 2 + NOO.factor() * 2.5 // keep bg rect in the middle
                spacing: NOO.factor()

                TipRect {
                    width: NOO.factor() * 12
                    height: NOO.factor() * 3
                    radius: height / 8
                    color: activPal.base

                    Text {
                        anchors.centerIn: parent
                        text: frequency
                        color: activPal.text

                        font {
                            pixelSize: NOO.factor() * 2
                            bold: true
                        }

                    }

                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: NOO.TR("SoundPage", "[Hz]")
                    color: activPal.text

                    font {
                        pixelSize: NOO.factor() * 2
                    }

                }

            }

        }

    }

    TcuteButton {
        x: NOO.factor() / 2
        y: NOO.factor() / 2
        height: NOO.factor() * 3
        visible: !exDrawer.visible
        text: "⋮"
        onClicked: exDrawer.open()

        font {
            pixelSize: NOO.factor() * 2
            bold: true
        }

    }

    Connections {
        target: SOUND
        function onVolumeUpPressed() : void {
            if (NOO.isAndroid())
                volSlider.value++;
        }
        function onVolumeDownPressed() : void {
            if (NOO.isAndroid())
                volSlider.value--;
        }
    }

}
