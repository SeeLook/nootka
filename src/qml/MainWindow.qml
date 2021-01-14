/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


ApplicationWindow {
  id: nootkaWindow
  visible: true
  title: "Nootka"
  color: activPal.window
  visibility: Noo.isAndroid() && GLOB.fullScreen() ? "FullScreen" : "AutomaticVisibility"

  property alias mainMenu: mainMenu

  // private
  property var noteName: null
  property var examResults: null
  property var executor: null
  property var dialogLoader: null
  property var tip: null
  property var analyzeWindow: null

  SystemPalette {
    id: activPal
    property color dimText: Qt.tint(activPal.base, Noo.alpha(activPal.text, 150))
    colorGroup: SystemPalette.Active
  }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  Connections {
    target: Noo
    onScoreActTriggered: mainMenu.open()
    onMelodyActTriggered: mainMenu.melodyOpen()
    onSettingsActTriggered: showDialog(Nootka.Settings)
    onLevelActTriggered: showDialog(Nootka.LevelCreator)
    onChartsActTriggered: showDialog(Nootka.Charts)
    onExamActTriggered: showDialog(Nootka.ExamStart)

    onAboutActTriggered: showDialog(Nootka.About)
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
          if (!executor)
            executor = Qt.createComponent("qrc:/exam/ExamExecutor.qml").createObject()
          if (!examResults)
            examResults = Qt.createComponent("qrc:/exam/ExamResults.qml").createObject(nootkaWindow.contentItem)
      } else {
          if (examResults)
            examResults.destroy()
      }
    }
    onWantOpenFile: {
      showDialog(Nootka.NoDialog)
      dialogLoader.openFile(fileName)
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
    if (!dialogLoader)
      dialogLoader = Qt.createComponent("qrc:/DialogLoader.qml").createObject(nootkaWindow)
    if (page === Nootka.Charts) { // dialogLoader above registers QML types in C++ - keep that order!
      analyzeWindow = Qt.createComponent("qrc:/charts/AnalyzeDialog.qml").createObject(nootkaWindow)
      return
    }
    dialogLoader.page = page
  }

  onClosing: {
    if (Noo.isAndroid() && dialogLoader && dialogLoader.visible) {
      close.accepted = false
      dialogLoader.close()
    }
    if (executor) {
      if (!executor.closeNootka())
        close.accepted = false
    }
    if (close.accepted)
      GLOB.geometry = Qt.rect(x ,y, width, height)
  }

  function audioAnalyze() {
    Qt.createComponent("qrc:/nootini/AudioAnalyze.qml").createObject(nootkaWindow)
    nootkaWindow.title = "Nootini - pitch detection analyzer"
  }
}
