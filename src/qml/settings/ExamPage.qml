/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  contentHeight: examCol.height + Noo.fontSize() * 2
  contentWidth: width

  property int mode: 0 // 0 - settings, 1 - exam, 2 - exercise

  Column {
    id: examCol
    width: parent.width; topPadding: Noo.fontSize()
    spacing: Noo.fontSize()

    Tframe {
      width: parent.width * 0.98
      anchors.horizontalCenter: parent.horizontalCenter
      Column {
        width: parent.width
        spacing: Noo.isAndroid() ? 2 : Noo.fontSize() / 2
        Tile {
          visible: mode === 0
          description: qsTr("Default name for every new exam or exercise.")
          Row {
            spacing: Noo.fontSize()
            anchors.horizontalCenter: parent.horizontalCenter
            Text { text: Noo.TR("StartExam", "student name:"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
            TtextField {
              id: userNameIn
              anchors.verticalCenter: parent.verticalCenter
              placeholderText: Noo.TR("StartExam", "Enter your name or nick-name.")
              font.pixelSize: Noo.fontSize(); maximumLength: 40
              width: Noo.fontSize() * 25
              horizontalAlignment: TextInput.AlignHCenter
              text: GLOB.student
            }
          }
        }
        Grid {
          columns: expertAnswChB.width > parent.width / 2 ? 1 : 2
          anchors.horizontalCenter: parent.horizontalCenter
          Tile {
            width: autoNextChB.width * 1.2
            anchors.horizontalCenter: undefined
            TcheckBox {
              id: autoNextChB
              anchors.horizontalCenter: parent.horizontalCenter
              text: qsTranslate("TexamHelp", "ask next question automatically")
            }
          }
          Tile {
            width: expertAnswChB.width * 1.2
            anchors.horizontalCenter: undefined
            TcheckBox {
              id: expertAnswChB
              anchors.horizontalCenter: parent.horizontalCenter
              text: qsTranslate("TexamHelp", "check answers without requiring confirmation")
              onClicked: {
                if (expertAnswChB.checked) {
                  expertDialog.open()
                }
              }
            }
          }
        }
        Tile {
          visible: mode === 0 || mode === 2
          TcheckBox {
            id: correctChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTranslate("TexamHelp", "correct mistakes (exercises only)")
          }
          description: qsTr("When you will make mistake, the program will show you automatically how a correct answer should be.")
        }
        Grid {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: Noo.fontSize() * 2
          columns: wrongColor.parent.width + correctColor.parent.width + notBadColor.parent.width + Noo.fontSize() * 4 < parent.width ? 3
                    : (wrongColor.parent.width + correctColor.parent.width + spacing < parent.width ? 2 : 1)

          Row {
            spacing: Noo.fontSize() / 2
            TlabelText { text: qsTr("color of questions") }
            ColorButton { id: wrongColor; color: GLOB.wrongColor; title: qsTr("color of questions") }
          }
          Row {
            spacing: Noo.fontSize() / 2
            TlabelText { text: qsTr("color of answers") }
            ColorButton { id: correctColor; color: GLOB.correctColor; title: qsTr("color of answers") }
          }
          Row {
            spacing: Noo.fontSize() / 2
            TlabelText { text: qsTr("color of 'not bad' answers") }
            ColorButton { id: notBadColor; color: GLOB.notBadColor; title: qsTr("color of 'not bad' answers") }
          }
        }
      }
    }

    Tframe { // Exercises frame
      visible: mode === 0 || mode === 2
      width: parent.width * 0.98
      anchors.horizontalCenter: parent.horizontalCenter
      Column {
        width: parent.width
        spacing: Noo.isAndroid() ? 2 : Noo.fontSize() / 2
        Text { text: Noo.TR("TexamHelp", "Exercises"); color: activPal.text; font.bold: true }
        Tile {
          TcheckBox {
            id: suggestChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("suggest an exam")
          }
          description: qsTr("Watch exercising progress and when it is going well, suggest to start an exam on the exercise level.")
        }
        Tile {
          TcheckBox {
            id: waitForCorrChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("wait for correct note")
          }
          description: qsTr("When melody is played from score, every note is checked immediately and next note can be played only when the previous one was correct. When unset, notes are checked only after playing the last one (or check button was clicked).")
        }
        Tile {
          TcheckBox {
            id: showWrongChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("show wrong played")
          }
          description: qsTr("When answer was played (or sung) and it was wrong also the detected wrong note is shown.")
        }
        Tile {
          TcheckBox {
            id: extraNamesChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("extra names")
          }
          description: qsTr("To improve association of note in the score or position on the instrument to note name, Nootka will display names even if neither question nor answer is related to it.")
        }
      }
    }

    Tframe { // Exam frame
      visible: mode === 0 || mode === 1
      width: parent.width * 0.98
      anchors.horizontalCenter: parent.horizontalCenter
      Column {
        width: parent.width
        spacing: Noo.isAndroid() ? 2 : Noo.fontSize() / 2
        Text { text: Noo.TR("TexamHelp", "Exams"); color: activPal.text; font.bold: true }
        Tile {
          TcheckBox {
            id: repeatChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("repeat a question")
          }
          description: qsTr("A question with an incorrect answer will be asked once again.")
        }
        Tile {
          TcheckBox {
            id: noConfirmChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("close without confirm")
          }
          description: qsTr("If checked, an application will not ask to answer pending question just mark it as wrong, save an exam to file (in directory: %1) and close itself without any confirmation needed.").arg("<b>" + GLOB.examsDir + "</b>")
        }
      }
    }

  }

  TpopupDialog {
    id: expertDialog
    bgColor: Qt.tint(activPal.window, Noo.alpha("red", 20))
    border { color: "red"; width: Noo.fontSize() / 4.0 }
    modal: true
    Tflickable {
      height: parent.height; contentHeight: expCol.height
      Column {
        id: expCol; width: parent.width; spacing: Noo.fontSize()
        Text {
          text: qsTr("Experts mode")
          color: activPal.text; font { pixelSize: Noo.fontSize() * 2; bold: true }
          anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
          width: parent.width; color: activPal.text
          wrapMode: Text.WordWrap; textFormat: Text.StyledText; horizontalAlignment: Text.AlignHCenter
          text: "<h4>" + qsTr("You are about to enter expert mode.<br> In this mode you don't need to confirm every answer,<br><b>but remember the following:") + "</b></h4><ul><li>" + qsTr("Selecting a note on the score or a position on the fingerboard invokes automatic checking of your answer, so select a key signature first, if required.") + "<br></li><li>" + qsTr("When an answer is the name of a note <b>first select a proper accidental and an octave</b> and then click a note button - this automatically invokes checking.") + "<br></li><li>" + qsTr("When you have to play a note as an answer - the first detected sound will be taken, so be sure that your input device captures exactly what you want.") + "<br></li><li>" + qsTr("When the last note of question that is a melody was played - checking starts.") + "<br></li></ul>"
        }
      }
    }
    onRejected: expertAnswChB.checked = false
  }

  Component.onCompleted: {
    autoNextChB.checked = GLOB.autoNextQuestion
    expertAnswChB.checked = GLOB.expertAnswers
    correctChB.checked = GLOB.correctAnswers
    suggestChB.checked = GLOB.suggestExam
    waitForCorrChB.checked = GLOB.waitForCorrect
    showWrongChB.checked = GLOB.showWrongPlayed
    extraNamesChB.checked = GLOB.extraNames
    repeatChB.checked = GLOB.repeatIncorect
    noConfirmChB.checked = GLOB.closeWithoutConfirm
  }

  function save() {
    GLOB.autoNextQuestion = autoNextChB.checked
    GLOB.expertAnswers = expertAnswChB.checked
    if (mode === 0 || mode === 2) {
      GLOB.suggestExam = suggestChB.checked
      GLOB.waitForCorrect = waitForCorrChB.checked
      GLOB.showWrongPlayed = showWrongChB.checked
      GLOB.extraNames = extraNamesChB.checked
      GLOB.correctAnswers = correctChB.checked
    }
    if (mode === 0 || mode === 1) {
      GLOB.closeWithoutConfirm = noConfirmChB.checked
      GLOB.repeatIncorect = repeatChB.checked
    }
    GLOB.wrongColor = wrongColor.color
    GLOB.notBadColor = notBadColor.color
    GLOB.correctColor = correctColor.color
    if (mode === 0)
      GLOB.student = userNameIn.text
  }

  function defaults() {
    autoNextChB.checked = false
    expertAnswChB.checked = false
    correctChB.checked = true
    suggestChB.checked = true
    waitForCorrChB.checked = true
    showWrongChB.checked = false
    extraNamesChB.checked = true
    repeatChB.checked = true
    noConfirmChB.checked = false
    wrongColor.color = Qt.rgba(1, 0, 0, 1)
    notBadColor.color = Qt.rgba(1, 0.5, 0, 1)
    correctColor.color = Qt.rgba(0, 1, 0, 1)
    userNameIn.text = GLOB.student
  }
}
