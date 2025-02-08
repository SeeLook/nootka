/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../score"
import Nootka.Charts 1.0
import QtQuick 2.12
import Score 1.0

TipRect {
    id: questTip

    property alias tipItem: tipItem

    width: tipItem.width
    height: tipItem.height
    z: 5000
    x: NOO.bound(parent.height / 12, tipItem.pos.x - chartView.list.contentX, parent.width - width * 1.2)
    y: tipItem.pos.y > parent.height / 2 ? tipItem.pos.y - height - parent.height / 14 : tipItem.pos.y + parent.height / 20
    shadowRadius: NOO.factor() * 3
    horizontalOffset: NOO.factor() / 3
    verticalOffset: horizontalOffset
    visible: scale > 0
    color: Qt.tint(activPal.base, NOO.alpha(tipItem.color, 50))
    radius: NOO.factor()
    scale: tipItem.show ? 1 : 0

    border {
        color: tipItem.color
        width: NOO.factor() / 7
    }

    // overlay area to catch when mouse exits a tip and hide it
    MouseArea {
        id: overArea

        parent: questTip.parent
        anchors.fill: parent
        z: 4999
        visible: false
        hoverEnabled: true
        onEntered: {
            visible = false;
            chartItem.tipExited();
        }
    }

    TchartTipItem {
        id: tipItem

        property bool prevShown: false //*< @p TRUE when user displayed entire melody

        width: (lineCol.visible ? lineCol.width : Math.max(scoreRow.width, headRow.width)) + NOO.factor()
        height: tipCol.height + NOO.factor()
        onQuestionWasSet: {
            attemptSpin.value = 0;
            prevShown = false;
        }

        Column {
            id: tipCol

            spacing: NOO.factor() / 2

            Row {
                id: headRow

                visible: tipItem.tipType === 0
                spacing: NOO.factor() * 2
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: tipItem.number + "."
                    color: activPal.text

                    font {
                        bold: true
                        pixelSize: NOO.factor() * 1.6
                    }

                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: tipItem.qaText
                    horizontalAlignment: Text.AlignHCenter
                    color: activPal.text

                    font {
                        pixelSize: NOO.factor() * 0.8
                    }

                }

            }

            Rectangle {
                id: scoreRow

                visible: tipItem.leftScoreVisible && tipItem.tipType === 0
                x: NOO.factor() / 2
                color: activPal.base
                height: tipItem.leftScoreHeight - NOO.factor()
                width: NOO.factor() * 40
                clip: true

                Score {
                    id: score

                    y: tipItem.yScoreLeftOff
                    width: parent.width
                    height: NOO.factor() * 15
                    Component.onCompleted: {
                        bgRect.destroy();
                        tipItem.leftScore = scoreObj;
                    }
                }

                Rectangle {
                    height: parent.height
                    width: parent.height
                    rotation: -90
                    anchors.right: parent.right
                    visible: tipItem.moreMelody && !tipItem.prevShown

                    RectButton {
                        // onClicked performed by mouse area

                        anchors.horizontalCenter: parent.horizontalCenter
                        y: parent.height * 0.66
                        height: parent.height / 3
                        rotation: 90
                        text: "\u0191"
                        textColor: activPal.text

                        font {
                            family: "Nootka"
                            pixelSize: parent.height / 3
                        }

                    }

                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "transparent"
                        }

                        GradientStop {
                            position: 0.5
                            color: activPal.base
                        }

                    }

                }

            }

            TspinBox {
                id: attemptSpin

                function pressed(m) {
                    if (m.x > width / 2)
                        up.pressed = true;
                    else
                        down.pressed = true;
                }

                function released() {
                    if (down.pressed || up.pressed) {
                        var prevV = value;
                        if (up.pressed)
                            increase();
                        else
                            decrease();
                        if (prevV !== value)
                            tipItem.setAttemptNr(value);

                    }
                    up.pressed = false;
                    down.pressed = false;
                }

                anchors.horizontalCenter: parent.horizontalCenter
                visible: tipItem.isMelody && tipItem.tipType === 0
                width: NOO.factor() * 15
                font.pixelSize: NOO.factor() * 0.8
                from: 0
                to: tipItem.attempts
                textFromValue: function(value) {
                    return NOO.TR("Texam", "attempt") + " " + value + " " + qsTranslate("ChartTip", "of", "It will give text: 'Attempt x of y'") + " " + to;
                }
            }

            Column {
                id: resultCol

                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - NOO.factor()
                visible: tipItem.tipType === 0

                Text {
                    width: parent.width
                    visible: text !== ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: tipItem.attemptDetails(attemptSpin.value)
                    textFormat: Text.StyledText
                    color: activPal.text
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: NOO.factor() * 0.9
                }

                Text {
                    width: parent.width // visible: text !== ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: tipItem.attemptResult(attemptSpin.value)
                    textFormat: Text.StyledText
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: NOO.factor() * 0.9
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: attemptSpin.value === 0 ? tipItem.resultText : ""
                    textFormat: Text.StyledText
                    color: tipItem.color
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: tipItem.timeText
                    textFormat: Text.StyledText
                    color: activPal.text
                    horizontalAlignment: Text.AlignHCenter

                    font {
                        pixelSize: NOO.factor() * 0.9
                    }

                }

            }

            // line tip column
            Column {
                id: lineCol

                padding: NOO.factor()
                visible: tipItem.tipType === 1

                Text {
                    width: NOO.factor() * 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: activPal.text
                    textFormat: Text.RichText
                    text: tipItem.tipText
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

            }

        }
        // tipItem

    }

    MouseArea {
        id: tipArea

        property real startPosY: 0
        property bool overScore: false

        anchors.fill: parent
        hoverEnabled: true
        onEntered: chartItem.tipEntered()
        onExited: overArea.visible = true
        onPressed: {
            startPosY = mouseY;
            if (tipCol.childAt(mouse.x, mouse.y) !== scoreRow)
                attemptSpin.pressed(mapToItem(attemptSpin, mouse.x, mouse.y));
            else if (tipItem.moreMelody && tipItem.prevShown)
                overScore = true;
        }
        onPositionChanged: {
            if (overScore) {
                score.contentY -= (mouseY - startPosY);
                startPosY = mouseY;
            }
        }
        onReleased: {
            if (tipCol.childAt(mouse.x, mouse.y) === scoreRow) {
                if (tipItem.moreMelody && !tipItem.prevShown) {
                    tipItem.showMelodyPreview();
                    tipItem.prevShown = true;
                }
            } else {
                attemptSpin.released();
            }
            if (overScore) {
                overScore = false;
                score.returnToBounds();
            }
        }
    }

    Behavior on scale {
        NumberAnimation {
            duration: 200
        }

    }

    Behavior on x {
        NumberAnimation {
            duration: 200
        }

    }

    Behavior on y {
        NumberAnimation {
            duration: 200
        }

    }

}
