/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TrtmSelectorItem {
    implicitWidth: NOO.factor() * 10
    implicitHeight: NOO.factor() * 20

    Component {
        id: moreComp

        Rectangle {
            height: NOO.factor() * 2
            width: NOO.factor() * 10
            color: enabled ? (ma.containsMouse ? activPal.highlight : activPal.button) : Qt.darker(disdPal.window, 1.2)

            Text {
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                width: NOO.factor() * 10
                fontSizeMode: Text.HorizontalFit
                text: qsTr("rhythmic groups") + "  ⋮"
                textFormat: Text.StyledText
                color: enabled ? (ma.containsMouse ? activPal.highlightedText : activPal.buttonText) : disdPal.buttonText

                font {
                    bold: true
                    pixelSize: NOO.factor()
                }

            }

            MouseArea {
                id: ma

                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    morePop.y = parent.y > 100 ? rtmList.height - morePop.height : rtmList.y;
                    morePop.open();
                }
            }

        }

    }

    ListView {
        id: rtmList

        clip: true
        height: parent.height
        width: parent.width
        spacing: NOO.factor() / 10
        model: 35
        header: moreComp
        footer: moreComp

        ScrollBar.vertical: ScrollBar {
            active: enabled
            visible: active
        }

        delegate: Component {
            Rectangle {
                height: NOO.factor() * 2.5
                width: parent ? parent.width : 0
                color: enabled ? (index % 2 ? activPal.alternateBase : activPal.base) : Qt.darker(disdPal.window, 1.2)

                MouseArea {
                    anchors.fill: parent
                    onClicked: userChangedGroup(index, !chBox.checked)
                }

                Row {
                    TcheckBox {
                        id: chBox

                        property int pow: Math.pow(2, index < 22 ? index : index - 22)

                        checked: (index < 22 && basicMask & pow) || (index > 21 && dotsMask & pow)
                        onClicked: userChangedGroup(index, checked)
                    }

                    Text {
                        y: -font.pixelSize * 0.8
                        text: getGroupText(index + 1)
                        color: enabled ? activPal.text : disdPal.text

                        font {
                            pixelSize: NOO.factor() * 1.5
                            family: "Scorek"
                        }

                    }

                }

            }

        }

    }

    Popup {
        id: morePop

        x: rtmList.width
        margins: NOO.factor()
        scale: GLOB.useAnimations ? 0 : 1

        Column {
            spacing: NOO.factor() / 2

            Text {
                text: qsTr("Filters") + ":"
                color: activPal.text
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TcheckBox {
                text: qsTr("basic rhythms")
                checked: basicMask
                onClicked: {
                    basicMask = checked ? 4.1943e+06 : 0;
                    rtmList.positionViewAtIndex(0, ListView.Beginning);
                    basicGroupChanged();
                }
            }

            TcheckBox {
                text: qsTr("rhythms with dots")
                checked: dotsMask
                onClicked: {
                    dotsMask = checked ? 16383 : 0;
                    rtmList.positionViewAtIndex(22, ListView.Beginning);
                    dotsGroupChanged();
                }
            }

        }

        background: GlowRect {
            color: activPal.window
            shadowRadius: NOO.factor() / 2
        }

        enter: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "scale"
                to: 1
            }

        }

        exit: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "scale"
                to: 0
            }

        }

    }

}
