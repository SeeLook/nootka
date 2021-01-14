/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Exam 1.0
import "../level"
import "../"


TstartExamItem {
  id: startDialog

  width: parent.width; height: parent.height

  Column {
    id: startCol
    width: parent.width; height: parent.height

    spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
    topPadding: Noo.fontSize() / 4

    Row {
      id: upperRow
      spacing: Noo.fontSize() * (Noo.isAndroid() ? 2 : 4)
      anchors.horizontalCenter: parent.horizontalCenter
      TiconButton {
        anchors.verticalCenter: parent.verticalCenter
        pixmap: Noo.pix("help"); iconHeight: startDialog.height / 15
        text: Noo.TR("QShortcut", "Help")
        onClicked: getHelpDialog()
      }
      Row {
        spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
        anchors.verticalCenter: parent.verticalCenter
        Text { text: qsTr("student name:"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
        TtextField {
          id: userNameIn
          anchors.verticalCenter: parent.verticalCenter
          placeholderText: qsTr("Enter your name or nick-name.")
          font.pixelSize: Noo.fontSize(); maximumLength: 40
          width: Noo.fontSize() * (Noo.isAndroid() ? 19 : 25)
          horizontalAlignment: TextInput.AlignHCenter
          text: GLOB.student
        }
      }
      TiconButton {
        anchors.verticalCenter: parent.verticalCenter
        pixmap: Noo.pix("levelCreator"); iconHeight: startDialog.height / 15
        text: Noo.TR("TlevelCreatorItem", "Level creator").replace(" ", "\n")
        onClicked: {
          dialLoader.close()
          nootkaWindow.showDialog(3) // Nootka.LevelCreator
        }
      }
    }

    Text {
      visible: !Noo.isAndroid()
      id: infoText
      text: qsTr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file.")
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width * 0.9; horizontalAlignment: Text.AlignHCenter; color: activPal.text
      font { bold: true; pixelSize: Noo.fontSize() * 0.8 }
      wrapMode: Text.WordWrap
    }

    LevelsSelector {
      id: selector
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width - Noo.fontSize()
      height: parent.height - upperRow.height - row1.height - row2.height
              - (Noo.isAndroid() ? startCol.spacing * 4 : infoText.height + startCol.spacing * 6)
    }

    Row {
      id: row1
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
      Tframe {
        width: startDialog.width / 2 - Noo.fontSize()
        Row {
          spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
          TiconButton {
            enabled: selector.levelId > -1 || prevLevelName() !== ""
            iconHeight: startDialog.height / 15
            pixmap: Noo.isAndroid() ? "" :  Noo.pix("practice")
            text: qsTr("Start exercise on level:")
            onClicked: start(Texecutor.StartExercise, selector.levelId > -1 ? selector.currentLevelVar() : prevLevel())
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "<b>" + (selector.levelId > -1 ? selector.levelName(selector.levelId) : (prevLevelName() === "" ? qsTr("No level was selected!") : prevLevelName())) + "</b>"
            color: activPal.text; font.pixelSize: Noo.fontSize() * 0.8
          }
        }
      }
      Tframe {
        width: startDialog.width / 2 - Noo.fontSize()
        Row {
          spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
          TiconButton {
            enabled: selector.levelId !== -1
            iconHeight: startDialog.height / 15
            pixmap: Noo.isAndroid() ? "" : Noo.pix("exam")
            text: qsTr("Pass new exam on level:")
            onClicked: start(Texecutor.NewExam, selector.currentLevelVar())
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "<b>" + (selector.levelId > -1 ? selector.levelName(selector.levelId) : qsTr("No level was selected!"))
            color: activPal.text; font.pixelSize: Noo.fontSize() * 0.8
          }
        }
      }
    }
    Item {
      id: row2
      anchors { horizontalCenter: parent.horizontalCenter }
      width: startDialog.width - Noo.fontSize() * 2; height: childrenRect.height
      Tframe {
        id: contFrame
        border.width: 0
        Row {
          TiconButton {
            iconHeight: startDialog.height / 15
            pixmap:  Noo.isAndroid() ? "" : Noo.pix("exam")
            text: qsTr("Select an exam to continue") + "   ⋮"
            onClicked: menu.open()
          }
        }
      }
      Tframe {
        id: lastExamFrame
        anchors {left: contFrame.right; leftMargin: Noo.fontSize() }
        width: parent.width - contFrame.width - exitFrame.width - 2 * Noo.fontSize()
        Row {
          spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
          TiconButton {
            id: lastExamButt
            iconHeight: startDialog.height / 15
            pixmap: Noo.isAndroid() ? "" : Noo.pix("exam")
            text: qsTr("Latest exam")
            enabled: lastExamFile !== ""
            onClicked: start(Texecutor.ContinueExam, lastExamFile)
          }
          Text {
            width: lastExamFrame.width - lastExamButt.width - 3 * Noo.fontSize()
            anchors.verticalCenter: parent.verticalCenter
            text: lastExamButt.enabled ? lastExamFile : Noo.TR("QPrintSettingsOutput", "None")
            color: activPal.text; font { pixelSize: Noo.fontSize() * 0.8; bold: true }
            wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2; elide: Text.ElideRight
          }
        }
      }
      Tframe {
        id: exitFrame
        anchors.right: parent.right; border.width: 0
        Row {
          TiconButton {
            iconHeight: startDialog.height / 15
            pixmap: Noo.pix("exit"); text: Noo.TR("QShortcut", "Exit")
            onClicked: dialLoader.close()
          }
        }
      }
    }
  }

  Tmenu {
    id: menu
    width: Noo.fontSize() * 20; y: startDialog.height * 0.88 - height; x: Noo.fontSize() * 2
    height: Math.min(startDialog.height * 0.8, contentItem.contentHeight)
    contentItem: ListView {
      clip: true
      ScrollBar.vertical: ScrollBar { active: true }
      model: recentModel
      delegate: MenuButton {
        width: Noo.fontSize() * 20
        action: modelData
        onClicked: menu.close()
        Rectangle { width: parent.width; height: index === recentModel.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
      }
    }
  }

  SequentialAnimation {
    id: noNameAnim
    loops: Animation.Infinite
    ColorAnimation { target: userNameIn.bg; property: "color"; to: "red"; duration: 500 }
    PauseAnimation { duration: 300 }
    ColorAnimation { target: userNameIn.bg; property: "color"; to: activPal.base; duration: 500 }
    PauseAnimation { duration: 300 }
  }

  Component.onCompleted: {
    dialLoader.standardButtons = 0
    dialLoader.title = qsTr("Start exercises or an exam")
  }

  onContinueExam: start(Texecutor.ContinueExam, examFile)

  function start(action, argument) {
    if (userNameIn.text === "") {
      noNameAnim.running = true
      giveUserNameMessage()
      noNameAnim.running = false
      userNameIn.bg.color = activPal.base
      return
    }
    GLOB.student = userNameIn.text
    GLOB.isExam = true
    if (!nootkaWindow.executor.init(action, argument)) {
        console.log("Executor discarded, deleting it")
        nootkaWindow.executor.destroy()
        GLOB.isExam = false
    }
    dialLoader.close()
  }
}
