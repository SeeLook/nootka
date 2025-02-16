/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../score"
import Nootka 1.0
import Nootka.Main 1.0
import QtQuick 2.12

GotIt {
    id: scoreHow

    /**
   * NOTICE: QML animation is tricky...
   * All x, y animated properties are saved before animation starts
   * and are fixed during all cycle.
   * So they has to be known and set properly.
   * This is why we are 'imitating' score toolbox @p factor - it is not known yet when anim starts.
   */
    property real factor: gotScore.scoreToobox ? gotScore.scoreToobox.factor * 0.9 : NOO.isAndroid() ? NOO.shortScreenSide() * 0.036 : NOO.factor() * 1.08
    property real editPosX: gotScore.x + 23 * gotScore.scale
    property real editPosY: gotScore.y + (gotScore.upperLine + 8) * gotScore.scale // upperLine + 4
    property real boxPosX: gotScore.x + 2 + factor * 2.5 + finger.width * 0.2 // in the middle of sharp control on score toolbox
    property real boxPosY: (NOO.isAndroid() ? (height - factor * 27) / 2 : NOO.factor() / 2) + factor * 7.5 + finger.width * 0.2

    function stop() {
        gotAnim.running = false;
    }

    function start() {
        finger.opacity = 0;
        finger.scale = 1;
        finger.x = parent.width - finger.width;
        finger.y = parent.height - finger.height;
        gotScore.noteAdd.active = false;
        gotScore.scoreToobox.active = false;
        gotScore.clearScore();
        gotScore.noteAdd.yPos = 0;
        gotIt.setCursorAlter(0);
        gotIt.setWorkRtmValue(3);
        descView.currentIndex = -1;
        gotAnim.running = true;
    }

    gotItType: TgotIt.GotHandleScore
    visible: true
    onClicked: close()

    Image {
        id: finger

        z: 1050 // above everything and score toolbox
        x: parent.width - width
        y: parent.height - height
        source: NOO.pix("fingerpoint")
        width: parent.width * 0.05
        height: width
        opacity: 0
    }

    // under score
    Rectangle {
        width: gotScore.width * 1.41
        height: width
        radius: width / 2
        x: width * -0.25
        y: height * -0.25
        color: NOO.alpha(activPal.highlight, 50)
    }

    // cover score mouse/touch actions
    MouseArea {
        anchors.fill: gotScore
        z: 1049
        hoverEnabled: true
    }

    Score {
        id: gotScore

        height: parent.width * 0.35
        width: height * 1.1
        bgColor: "transparent"
        enableKeySign: true
        enableDoubleAccids: true
        scoreObj.enableDoubleAccidentals: true
        firstStaff.scale: height / firstStaff.linesCount
        readOnly: false // HACK
        scoreObj.editMode: true
        scoreObj.allowAdding: visible
        Component.onCompleted: gotIt.score = scoreObj
    }

    Rectangle {
        width: parent.width * 0.59
        height: parent.height - NOO.factor() * 4
        x: parent.width * 0.41 + NOO.factor()
        y: NOO.factor()

        Column {
            anchors.horizontalCenter: parent.hozrizontalCenter
            width: parent.width - NOO.factor() * 2
            spacing: NOO.factor()

            Text {
                anchors.horizontalCenter: parent.hozrizontalCenter
                color: activPal.text
                text: qsTr("Editing score with touch")

                font {
                    bold: true
                    pixelSize: scoreHow.width * 0.03
                }

            }

            ListView {
                id: descView

                currentIndex: -1
                clip: true
                model: [qsTr("Touch and hold area of a note you want to edit."), qsTr("Move finger left or right to see edited note."), qsTr("Then move finger up or down to find desired note pitch."), qsTr("Rise the finger."), qsTr("If necessary, select accidental or rhythmic value."), qsTr("Finally, tap selected note shortly.")]
                width: parent.width - NOO.factor() * 2
                height: parent.parent.height - scoreHow.width * 0.04 - NOO.factor()
                spacing: NOO.factor() / 2

                delegate: Text {
                    width: parent.width - NOO.factor() / 2
                    text: index + 1 + ". " + modelData
                    color: activPal.text
                    wrapMode: Text.WordWrap
                    style: index === descView.currentIndex ? Text.Sunken : Text.Normal
                    styleColor: activPal.highlight

                    font {
                        bold: index === descView.currentIndex
                        pixelSize: Math.max(scoreHow.width * 0.015, NOO.factor())
                    }

                }

            }

        }

    }

    // run animation with delay to allow initialize all properties
    Timer {
        running: true
        interval: 250
        onTriggered: gotAnim.running = true
    }

    SequentialAnimation {
        id: gotAnim

        PauseAnimation {
            duration: 500
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "opacity"
                to: 1
            }

            NumberAnimation {
                target: finger
                property: "scale"
                to: 3
                easing.type: Easing.OutBack
                duration: 1000
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: editPosX
                duration: 1000
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY
                easing.type: Easing.OutBack
                duration: 1000
            }

        }

        ScriptAction {
            script: {
                descView.currentIndex = 0; // touch and hold
                gotScore.noteAdd.visible = true;
                gotScore.scoreToobox.active = false;
                gotScore.noteAdd.active = true;
                gotScore.noteAdd.yPos = gotScore.upperLine + 4;
            }
        }

        PauseAnimation {
            duration: 1000
        }
        // Move left or right

        ScriptAction {
            script: descView.currentIndex = 1
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "x"
                to: editPosX - scoreHow.width * 0.05
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY
                duration: 500
            }

        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "x"
                to: editPosX + scoreHow.width * 0.05
                duration: 1000
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY
                duration: 1000
            }

        }

        PauseAnimation {
            duration: 1000
        }
        // Move up and down

        ScriptAction {
            script: descView.currentIndex = 2
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY - 10 * gotScore.scale
                duration: 1200
            }

            NumberAnimation {
                target: gotScore.noteAdd
                property: "yPos"
                to: gotScore.upperLine - 6
                duration: 1200
            }

        }

        PauseAnimation {
            duration: 500
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY + 8 * gotScore.scale
                duration: 2000
            }

            NumberAnimation {
                target: gotScore.noteAdd
                property: "yPos"
                to: gotScore.upperLine + 12
                duration: 2000
            }

        }

        PauseAnimation {
            duration: 500
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY + 2 * gotScore.scale
                duration: 600
            }

            NumberAnimation {
                target: gotScore.noteAdd
                property: "yPos"
                to: gotScore.upperLine + 6
                duration: 600
            }

        }
        // Rise finger

        ScriptAction {
            script: descView.currentIndex = 3
        }

        PauseAnimation {
            duration: 1000
        }

        ScriptAction {
            script: gotScore.scoreToobox.active = true
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 4
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: editPosX + scoreHow.width * 0.07
                duration: 500
            }

        }
        // select accidental

        ScriptAction {
            script: descView.currentIndex = 4
        }

        PauseAnimation {
            duration: 1000
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 3
                duration: 750
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: boxPosX
                duration: 750
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: boxPosY
                duration: 750
            }

        }
        // select sharp

        ScriptAction {
            script: gotIt.setCursorAlter(1)
        }

        PauseAnimation {
            duration: 1000
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 3
                easing.type: Easing.OutBack
                duration: 750
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: boxPosX + factor * 2
                easing.type: Easing.OutBack
                duration: 750
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: boxPosY + factor * 12
                duration: 750
            }

        }
        // select eight

        ScriptAction {
            script: gotIt.setWorkRtmValue(4)
        }

        PauseAnimation {
            duration: 1000
        }
        // finally confirm note

        ScriptAction {
            script: descView.currentIndex = 5
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 4
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: boxPosX + (editPosX - boxPosX) / 2
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY - gotScore.height * 0.2
                duration: 500
            }

        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 3
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: editPosX
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: editPosY
                duration: 500
            }

        }

        ScriptAction {
            script: {
                gotScore.addNote(NOO.note(5, 1, 1, 4)); // add note to e score (g#1 eight)
                gotScore.scoreToobox.active = false;
                gotScore.noteAdd.active = false;
            }
        }

        PauseAnimation {
            duration: 150
        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "scale"
                to: 4
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "x"
                to: scoreHow.width * 0.26
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: scoreHow.height * 0.3
                duration: 500
            }

        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "x"
                to: scoreHow.width * 0.32
                duration: 500
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: scoreHow.height * 0.39
                duration: 500
            }

        }

        ParallelAnimation {
            NumberAnimation {
                target: finger
                property: "x"
                to: scoreHow.width * 0.8
                duration: 1000
            }

            NumberAnimation {
                target: finger
                property: "y"
                to: scoreHow.height * 0.8
                duration: 1000
            }

        }

        ScriptAction {
            script: gotScore.noteAdd.visible = false
        }

        PauseAnimation {
            duration: 1000
        }

        ScriptAction {
            script: descView.currentIndex = -1
        }

    }

    Row {
        x: NOO.factor() * (NOO.isAndroid() ? 1 : 2)
        y: parent.height - height - NOO.factor() / 2
        spacing: NOO.factor() * (NOO.isAndroid() ? 1 : 3)

        TcuteButton {
            text: NOO.TR("QShortcut", gotAnim.running ? "Stop" : "Play")
            width: height * 3.5
            height: NOO.factor() * (NOO.isAndroid() ? 2 : 3)
            onClicked: {
                if (gotAnim.running)
                    scoreHow.stop();
                else
                    scoreHow.start();
            }

            font {
                pixelSize: height * 0.7
                bold: true
                capitalization: Font.AllUppercase
            }

        }

        TcuteButton {
            enabled: gotAnim.running
            text: gotAnim.paused ? NOO.TR("QWizard", "Continue") : NOO.TR("QShortcut", "Pause")
            width: height * 3.5
            height: NOO.factor() * (NOO.isAndroid() ? 2 : 3)
            onClicked: {
                if (gotAnim.paused)
                    gotAnim.resume();
                else
                    gotAnim.pause();
            }

            font {
                pixelSize: height * 0.7
                bold: true
                capitalization: Font.AllUppercase
            }

        }

    }

}
