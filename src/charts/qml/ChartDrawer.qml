/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Drawer {
    // Tflickable

    id: chartDrawer

    property alias pinned: pinBox.checked
    property int fSize: Math.min(NOO.factor(), width / 25)

    visible: true
    modal: false
    y: analyzeWindow.height / 15 + Screen.pixelDensity * 2
    width: parent.height * 0.4
    height: parent.height - y
    closePolicy: chartItem.keepDrawerOpened() ? Popup.NoAutoClose : Popup.CloseOnEscape | Popup.CloseOnPressOutside

    Connections {
        target: chartItem
        function onLockXorderList(itNr: int, state: bool) : void { orderCombo.lock(itNr, state) }
    }

    Tflickable {
        height: parent.height
        contentHeight: drawCol.height

        Column {
            id: drawCol

            width: parent.width
            spacing: fSize / 2

            Column {
                visible: chartItem.allowOpen
                width: parent.width

                MenuButton {
                    width: parent.width
                    action: chartItem.loadExamAct()
                }

                ListView {
                    id: recentList

                    width: parent.width // 6 items visible
                    height: Math.min(NOO.factor() * 16.8, NOO.factor() * count * 2.8)
                    boundsBehavior: Flickable.StopAtBounds
                    contentHeight: NOO.factor() * count * 2.8
                    model: chartItem.recentExamsActions
                    currentIndex: chartItem.selectedFileId
                    clip: true

                    delegate: MenuButton {
                        width: parent ? parent.width : 0
                        action: modelData
                        color: containsPress || recentList.currentIndex === index ? activPal.highlight : (containsMouse ? NOO.alpha(activPal.highlight, 50) : (index % 2 ? activPal.alternateBase : activPal.base))
                        textColor: containsPress || recentList.currentIndex === index ? activPal.highlightedText : activPal.text

                        Rectangle {
                            width: parent.width
                            height: 1
                            color: activPal.window
                            y: parent.height - 1
                        }

                    }

                    ScrollBar.vertical: ScrollBar {
                        active: true
                        visible: true
                    }

                }

                MenuButton {
                    visible: chartItem.exerciseAct()
                    width: parent.width
                    action: chartItem.exerciseAct()

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: activPal.dimText
                        y: parent.height - 1
                    }

                }

            }

            Item {
                visible: !chartItem.isMelody
                width: parent.width
                height: fSize
            }

            Row {
                visible: !chartItem.isMelody
                spacing: fSize / 4
                anchors.horizontalCenter: parent.horizontalCenter

                TradioButton {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: fSize
                    checked: chartItem.chartType === 0
                    onClicked: chartItem.chartType = checked ? 0 : 1
                }

                Text {
                    text: qsTranslate("AnalyzeDialog", "linear chart").replace(" ", "\n")
                    anchors.verticalCenter: parent.verticalCenter
                    color: activPal.text
                    font.pixelSize: fSize
                }

                Item {
                    width: chartDrawer.width / 10
                    height: 1
                }

                TradioButton {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: fSize
                    onClicked: chartItem.chartType = checked ? 1 : 0
                }

                Text {
                    text: qsTranslate("AnalyzeDialog", "bar chart").replace(" ", "\n")
                    anchors.verticalCenter: parent.verticalCenter
                    color: activPal.text
                    font.pixelSize: fSize
                }

            }

            Item {
                width: parent.width
                height: fSize
            }

            Column {
                visible: !chartItem.isMelody
                width: parent.width
                spacing: fSize / 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTranslate("AnalyzeDialog", "ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc...")
                    color: activPal.text
                    font.pixelSize: fSize
                }

                TcomboBox {
                    id: orderCombo

                    width: Math.min(parent.width - fSize, NOO.factor() * 20)
                    font.pixelSize: fSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: chartItem.xOrderActions
                    currentIndex: chartItem.xOrder
                    onActivated: index => { chartItem.xOrder = index }
                }

            }

            Column {
                visible: chartItem.isMelody
                width: parent.width
                spacing: fSize / 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTranslate("AnalyzeDialog", "Y value:")
                    color: activPal.text
                    font.pixelSize: fSize
                }

                TcomboBox {
                    width: Math.min(parent.width - fSize, NOO.factor() * 20)
                    font.pixelSize: fSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: chartItem.yValueActions
                    currentIndex: chartItem.yValue
                    onActivated: chartItem.yValue = index
                }

            }

            Item {
                width: parent.width
                height: fSize
            }

            Column {
                width: parent.width
                spacing: fSize / 2

                Repeater {
                    model: chartItem.miscSettModel

                    Row {
                        spacing: fSize / 2
                        enabled: modelData.enabled

                        TcheckBox {
                            font.pixelSize: fSize
                            anchors.verticalCenter: parent.verticalCenter
                            checked: modelData.checked
                            onClicked: modelData.trigger()
                        }

                        Text {
                            width: chartDrawer.width - fSize * 4
                            anchors.verticalCenter: parent.verticalCenter
                            wrapMode: Text.Wrap
                            text: modelData.text
                            color: enabled ? activPal.text : disdPal.text
                            font.pixelSize: fSize
                        }

                    }

                }

            }

        }

        // Column
        ScrollBar.horizontal: ScrollBar {
            active: true
            visible: true
        }

    }

    TcheckBox {
        id: pinBox

        hoverEnabled: true
        font.pixelSize: fSize
        checked: chartItem.keepDrawerOpened()
        onClicked: {
            chartDrawer.closePolicy = checked ? Popup.NoAutoClose : Popup.CloseOnEscape | Popup.CloseOnPressOutside;
            chartItem.setKeepDrawer(checked);
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
        }

        ToolTip {
            delay: 500
            timeout: 5000
            visible: pinBox.hovered

            contentItem: Text {
                text: qsTr("keep opened")
                color: activPal.highlightedText
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

            background: TipRect {
                shadowRadius: NOO.factor()
                color: activPal.highlight
            }

        }

    }

    background: TipRect {
        color: activPal.window
        radius: 0
        verticalOffset: 0
        horizontalOffset: pinBox.checked ? 0 : NOO.factor() / 5
    }

}
