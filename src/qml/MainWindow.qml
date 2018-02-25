/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0


ApplicationWindow {
  id: nootkaWindow
  visible: true
  title: "Nootka"
  color: activPal.window

  property alias mainMenu: mainMenu
  property alias settingsAct: settingsAct
  property alias aboutAct: aboutAct
  property alias levelAct: levelAct
  property alias scoreAct: scoreAct
  property alias examAct: examAct

  // private
  property var noteName: null
  property var examResults: null
  property var executor: null
  property var dialogLoader: null

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  Taction {
    id: settingsAct
    icon: "systemsettings"
    text: qsTranslate("TtoolBar", "Settings")
    tip: qsTranslate("TtoolBar", "Application preferences")
    onTriggered: showDialog(Nootka.Settings)
  }
  Taction { id: aboutAct; onTriggered: showDialog(Nootka.About) }
  Taction {
    id: levelAct
    icon: "levelCreator"
    text: qsTranslate("TtoolBar", "Level")
    tip: qsTranslate("TtoolBar", "Levels creator")
    onTriggered: showDialog(Nootka.LevelCreator)
  }
  Taction { // desktop only
    id: scoreAct
    icon: "score"
    text: qsTranslate("TtoolBar", "Score", "it could be 'notation', 'staff' or whatever is associated with that 'place to display musical notes' and this the name is quite short and looks well.")
    tip: qsTranslate("TtoolBar", "Manage and navigate the score.")
    onTriggered: mainMenu.open()
  }
  Taction {
    id: examAct
    icon: "startExam"
    text: qsTranslate("TtoolBar", "Lessons")
    tip: qsTranslate("TtoolBar", "Start exercises or an exam")
    onTriggered: showDialog(Nootka.ExamStart)
  }

  width: GLOB.geometry.width
  height: GLOB.geometry.height
  x: GLOB.geometry.x
  y: GLOB.geometry.y

  MainMenu { id: mainMenu }
  header: mainMenu.toolBar

  MainScore {
    id: score
    y: examResults ? examResults.height + 2 : 0
    height: nootkaWindow.height - (header ? header.height : 0)
            - (GLOB.instrument.isSax ? (GLOB.singleNoteMode ? instrument.height / 7 : 0) : instrument.height)
            - (examResults ? examResults.height + 2 : 0)
    width: (parent.width - (GLOB.instrument.isSax ? instrument.width : 0)) / (GLOB.singleNoteMode ? 2 : 1)
    z: 5
  }

  Instrument {
    id: instrument
    score: score
  }

  Connections {
    target: GLOB
    onSingleNoteModeChanged: checkSingleMode()
    onIsExamChanged: {
      if (GLOB.isExam) {
          if (!executor) {
              var e = Qt.createComponent("qrc:/exam/ExamExecutor.qml")
              executor = e.createObject(nootkaWindow)
          }
          if (!examResults) {
              var c = Qt.createComponent("qrc:/exam/ExamResults.qml")
              examResults = c.createObject(nootkaWindow.contentItem)
          } else { // TODO remove when does not occur
              console.log("[MainWindow.qml] EXAM RESULTS ALREADY EXISTS!")
          }
      } else {
          if (examResults)
            examResults.destroy()
      }
    }
  }

  Component.onCompleted: {
    Noo.mainScore = score
    checkSingleMode()
  }

  function checkSingleMode() {
    if (GLOB.singleNoteMode) {
        if (!noteName) {
          var c = Qt.createComponent("qrc:/NoteName.qml")
          noteName = c.createObject(nootkaWindow.contentItem, { "score": score })
        }
    } else {
        if (noteName)
          noteName.destroy()
    }
  }

  function showDialog(page) {
    if (!dialogLoader) {
      var c = Qt.createComponent("qrc:/DialogLoader.qml")
      dialogLoader = c.createObject(nootkaWindow)
    }
    dialogLoader.page = page
  }

  onClosing: {
    if (executor) {
      if (!executor.closeNootka())
        close.accepted = false
    }
    if (close.accepted)
      GLOB.geometry = Qt.rect(x ,y, width, height)
  }
}
