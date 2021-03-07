/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Main 1.0
import "../score"
import "../"


GotIt {
  id: selectHow
  gotItType: TgotIt.GotNoteSelected
  visible: true

  onClicked: {
    if (showGotIt) { // Got It, not help dialog
      if (!remaindChecked) // store only if user doesn't want it next time
        GLOB.setGotIt("noteSelected", false)
      SOUND.startListen()
    }
    close()
  }

  property int noSelCnt: 1
  property int restAt: 4

  Tflickable {
    anchors.fill: parent
    contentHeight: selCol.height + NOO.factor() * 10
    Column {
      id: selCol
      width: parent.width
      spacing: NOO.factor() / 2
      Tile {
        id: selectedTile
        anchors { horizontalCenter: undefined; right: parent.right }
        width: parent.width * (NOO.isAndroid() ? 1 : 0.9)
        height: selText.height + selScore.height * 0.7
        bgBorder { color: activPal.highlight; width: 2 }
        bgColor: Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 20))

        Score {
          id: selScore
          anchors.horizontalCenter: parent.horizontalCenter
          y: -height * (NOO.isAndroid() ? 0.25 : 0.1)
          height: selectHow.width * 0.3; width: parent.width * 0.8
          bgColor: "transparent"; editText.visible: false
          readOnly: false; scoreObj.editMode: true; scoreObj.allowAdding: visible // HACK
          Component.onCompleted: {
            for (var n = 1; n < 8; ++n)
              addNote(NOO.note(n, 1, 0, (n < 2 ? 3 : 4)))
            currentNote = scoreObj.note(5)
          }
          Rectangle { // note highlight
            id: noteHighlight
            parent: selScore.currentNote
            width: selScore.currentNote ? selScore.currentNote.width * 1.5 : 0
            height: selScore.currentNote ? Math.min(14.0, selScore.currentNote.notePosY + 8.0) : 0
            x: selScore.currentNote ? -width * 0.25 : 0
            y: selScore.currentNote ? Math.min(selScore.currentNote.height - height, Math.max(0.0, selScore.currentNote.notePosY - height / 2.0)) : 0
            color: NOO.alpha(activPal.highlight, 75)
            z: -1
            radius: width / 3.0
          }
          Text {
            parent: selScore.noteAdd
            x: parent ? (parent.width - width) / 2 : 0; y: 14 - height * scale
            text: gotIt.noteCursorText()
            horizontalAlignment: Text.AlignHCenter
            color: GLOB.noteCursorColor
            font { pixelSize: 10; bold: true }
            scale: 0.2; transformOrigin: Item.Top
          }
          MouseArea { // cover score mouse/touch actions
            anchors.fill: parent
            z: 1049
            hoverEnabled: true
          }
          SequentialAnimation {
            id: selAnim
            loops: Animation.Infinite
            ScriptAction {
              script: selScore.setNote(5, NOO.note(1 + Math.random() * 7, 1 + Math.random() * 2, -1 + noSelCnt % 3, 4))
            }
            PauseAnimation { duration: 1500 }
          }
        }
        Text {
          id: selText
          anchors.horizontalCenter: parent.horizontalCenter
          y: selScore.height * 0.55; width: parent.width * (NOO.isAndroid() ? 0.9 : 0.7)
          color: activPal.text
          text: gotIt.ifSelectedText()
          wrapMode: Text.WordWrap; font.pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2)
        }

      }
      Tile {
        width: selectHow.width * (NOO.isAndroid() ? 0.9: 0.6)
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text
          width: parent.width - NOO.factor(); leftPadding: NOO.factor()
          text: gotIt.unselectText()
          wrapMode: Text.WordWrap; font.pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2)
        }
      }
      Tile {
        id: noSelTile
        width: parent.width * (NOO.isAndroid() ? 1 : 0.9)
        height: noSelText.height + noSelScore.height * 0.7
        anchors.horizontalCenter: undefined
        bgBorder { color: activPal.text; width: 2 }
        bgColor: activPal.base

        Score {
          id: noSelScore
          anchors.horizontalCenter: parent.horizontalCenter
          y: -height * (NOO.isAndroid() ? 0.25 : 0.1)
          height: selectHow.width * 0.3; width: parent.width * 0.8
          bgColor: "transparent"; editText.visible: false
          readOnly: false; scoreObj.editMode: true; scoreObj.allowAdding: visible
          Text {
            parent: noSelScore.noteAdd
            x: parent ? (parent.width - width) / 2 : 0; y: 14 - height * scale
            text: gotIt.noteCursorText()
            horizontalAlignment: Text.AlignHCenter
            color: GLOB.noteCursorColor
            font { pixelSize: 10; bold: true }
            scale: 0.2; transformOrigin: Item.Top
          }
          MouseArea {
            anchors.fill: parent
            z: 1049
            hoverEnabled: true
          }
          SequentialAnimation {
            id: noSelAnim
            loops: Animation.Infinite
            ScriptAction {
              script: {
                noSelScore.addNote(NOO.note(noSelCnt, 1, 0, 3 + Math.random() * 3, noSelCnt === restAt))
                noSelCnt++
                if (noSelCnt > 8) {
                  noSelScore.clearScore()
                  noSelCnt = 1
                  restAt = Math.random() * 8
                }
              }
            }
            PauseAnimation { duration: 1000 }
          }
        }
        Text {
          id: noSelText
          anchors.horizontalCenter: parent.horizontalCenter
          y: noSelScore.height * 0.55; width: parent.width * (NOO.isAndroid() ? 0.9 : 0.7)
          color: activPal.text
          text: gotIt.ifNotSelectedText()
          wrapMode: Text.WordWrap; font.pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2)
        }
      }
    }
  }

  Timer {
    running: true
    interval: 500
    onTriggered: {
      selAnim.running = true
      noSelAnim.running = true
      noSelScore.noteAdd.hiTimer.running = true
    }
  }

}
