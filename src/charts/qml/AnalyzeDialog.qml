/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Charts 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Window {
    id: analyzeWindow

    property alias exam: chartItem.exam
    property alias allowOpen: chartItem.allowOpen
    //* private
    property var helpTip: null
    property int upTextSize: Math.round(toolBar.height * 0.25)
    property var chDrawer: null

    function openExam() {
        if (allowOpen)
            chartItem.openExam();
    }

    function openDrawer() {
        drawerAct.trigger();
    }

    visible: true
    modality: Qt.WindowModal
    title: chartItem.chartWindowTitle
    width: nootkaWindow.width
    height: nootkaWindow.height
    x: nootkaWindow.x
    y: nootkaWindow.y
    color: activPal.base

    onClosing: {
        SOUND.startListen();
        analyzeWindow.destroy();
    }

    Component.onCompleted: {
        if (allowOpen) {
            var h = Qt.createComponent("qrc:/charts/ChartHelpTip.qml");
            helpTip = h.createObject(analyzeWindow.contentItem, {
                "text": chartItem.chartHelpText()
            });
        }
        SOUND.stop();
        if (chartItem.keepDrawerOpened())
            openDrawer();
    }

    ButtonGroup {
        id: chartTypeGr
    }

    Column {
        property TchartItem chartItem

        chartItem: TchartItem {
            id: chartItem

            parent: chartView
            anchors.fill: parent
            onExamChanged: {
                if (helpTip && exam)
                    helpTip.destroy();

                if (tipItem)
                    tipItem.parent.destroy();

                tipItem = Qt.createComponent("qrc:/charts/ChartTip" + (isMelody ? "Melody" : "") + ".qml").createObject(chartView.list).tipItem;
            }
            onResetChartPos: chartView.list.positionViewAtBeginning()
            onLoadExamFailed: {
                if (!helpTip)
                    helpTip = Qt.createComponent("qrc:/charts/ChartHelpTip.qml").createObject(analyzeWindow.contentItem);
                helpTip.text = message;
                helpTip.hintColor = GLOB.wrongColor;
            }
        }

        ToolBar {
            id: toolBar

            z: 255
            height: helpButt.height
            width: analyzeWindow.width

            Row {
                ChartToolButton {

                    taction: Taction {
                        id: drawerAct

                        text: qsTr("Settings of a chart")
                        icon: "chartSett"
                        onTriggered: {
                            if (chDrawer) {
                                if (chDrawer.visible)
                                    chDrawer.close();
                                else
                                    chDrawer.open();
                            } else {
                                var d = Qt.createComponent("qrc:/charts/ChartDrawer.qml");
                                chDrawer = d.createObject(analyzeWindow);
                            }
                        }
                    }
                }

                ChartToolButton {

                    taction: Taction {
                        text: NOO.TR("QShortcut", "Zoom In")
                        icon: "zoom-in"
                        onTriggered: chartView.sc = Math.min(2, chartView.sc * 1.125)

                        shortcut: Shortcut {
                            sequences: [ StandardKey.ZoomIn ]
                        }
                    }

                }

                ChartToolButton {

                    taction: Taction {
                        text: NOO.TR("QShortcut", "Zoom Out")
                        icon: "zoom-out"
                        onTriggered: chartView.sc = Math.max(0.5, chartView.sc * 0.888889)

                        shortcut: Shortcut {
                            sequence: StandardKey.ZoomOut
                        }

                    }

                }

                ChartToolButton {

                    taction: Taction {
                        text: NOO.TR("QMdiSubWindow", "Maximize")
                        icon: "fullscreen"
                        onTriggered: visibility = visibility === 2 ? 4 : 2
                    }

                }

                ChartToolButton {
                    id: helpButt

                    taction: Taction {
                        text: NOO.TR("QShortcut", "Help")
                        icon: "help"
                        onTriggered: NOO.openDocLink("2017/05/17/analyze-results/")
                    }

                }

                Item {
                    height: 2
                    width: (analyzeWindow.width - (helpButt.x + helpButt.width) - exitButt.width - infoRow.width) / 2
                }

                Row {
                    id: infoRow

                    //             visible: chartItem.questionCount > 0
                    Column {
                        visible: chartItem.isMelody // && chartItem.questionCount > 0

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("Y value:")
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            height: toolBar.height * 0.48

                            font {
                                pixelSize: upTextSize
                            }

                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: chartItem.yValueActions[chartItem.yValue]
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            height: toolBar.height * 0.48
                            width: analyzeWindow.width / 7
                            fontSizeMode: Text.Fit
                            minimumPixelSize: NOO.factor() / 2

                            font {
                                pixelSize: upTextSize
                                bold: true
                            }

                        }

                    }

                    Item {
                        height: 2
                        width: analyzeWindow.width / 100
                    }

                    Column {
                        visible: !chartItem.isMelody //&& chartItem.questionCount > 0

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc...")
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            height: toolBar.height * 0.48
                            width: analyzeWindow.width / 7

                            font {
                                pixelSize: upTextSize
                            }

                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: chartItem.xOrderActions[chartItem.xOrder]
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            height: toolBar.height * 0.48
                            width: analyzeWindow.width / 7
                            fontSizeMode: Text.Fit
                            minimumPixelSize: NOO.factor() / 2

                            font {
                                pixelSize: upTextSize
                                bold: true
                            }

                        }

                    }

                    Item {
                        height: 2
                        width: analyzeWindow.width / 100
                    }

                    Column {
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("student name:")
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            height: toolBar.height * 0.5

                            font {
                                pixelSize: upTextSize
                            }

                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: chartItem.userName === "" ? "-----" : chartItem.userName
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            height: toolBar.height * 0.5

                            font {
                                pixelSize: toolBar.height / 4
                                bold: true
                            }

                        }

                    }

                    Item {
                        height: 2
                        width: analyzeWindow.width / 100
                    }

                    Column {
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("level:")
                            color: activPal.text
                            verticalAlignment: Text.AlignVCenter
                            height: toolBar.height * 0.5

                            font {
                                pixelSize: upTextSize
                            }

                        }

                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: levelText.width + NOO.factor()
                            height: toolBar.height / 2
                            color: levelArea.containsMouse ? activPal.highlight : "transparent"
                            radius: height / 5

                            Text {
                                id: levelText

                                anchors.centerIn: parent
                                text: chartItem.levelName === "" ? "-----" : chartItem.levelName
                                color: activPal.text

                                font {
                                    pixelSize: toolBar.height / 4
                                    bold: true
                                }

                            }

                            MouseArea {
                                id: levelArea

                                hoverEnabled: chartItem.levelName !== ""
                                anchors.fill: parent
                                onClicked: {
                                    var c = Qt.createComponent("qrc:/charts/LevelPopup.qml");
                                    var lp = c.createObject(analyzeWindow.contentItem);
                                    chartItem.fillPreview(lp.levelPreview);
                                }
                            }

                        }

                    }

                    Item {
                        height: 2
                        width: analyzeWindow.width / 100
                    }

                    Column {
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: chartItem.questionCount
                            color: activPal.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            height: toolBar.height * 0.5

                            font {
                                pixelSize: upTextSize
                            }

                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: chartItem.effectiveness
                            color: activPal.text
                            horizontalAlignment: Text.AlignHCenter

                            font {
                                pixelSize: toolBar.height / 4
                            }

                        }

                    }
                    // Row (infoRow)

                }
                // Row

            }

            ChartToolButton {
                id: exitButt

                anchors.right: parent.right

                taction: Taction {
                    text: NOO.TR("QPlatformTheme", "Close")
                    icon: "exit"
                    onTriggered: close()
                }

            }

            background: TipRect {
                anchors.fill: parent
                color: activPal.window
                radius: 0
            }

        }

        ChartView {
            id: chartView

            width: analyzeWindow.width - (chDrawer && chDrawer.pinned && chDrawer.visible ? chDrawer.width : 0)
            height: analyzeWindow.height - toolBar.height

            transform: Translate {
                x: chDrawer ? chDrawer.position * analyzeWindow.height * 0.4 : 0
            }

        }

    }

}
