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
  visibility: NOO.isAndroid() && GLOB.fullScreen() ? "FullScreen" : "AutomaticVisibility"

  property alias mainMenu: mainMenu

  // private
  property var noteName: null
  property var examResults: null
  property var executor: null
  property var dialogLoader: null
  property var tip: null
  property var analyzeWindow: null
  property var sndInf: null

  SystemPalette {
    id: activPal
    property color dimText: Qt.tint(activPal.base, NOO.alpha(activPal.text, 150))
    colorGroup: SystemPalette.Active
  }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  Connections {
    target: NOO
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

  Item {
    id: scoreWrap
    y: examResults ? examResults.height + 2 : 0
    height: score.height
    width: parent.width * (GLOB.instrument.isSax ? 0.15 : 1)
    z: 5
    transformOrigin: Item.Top
    MainScore {
      id: score
      property real insHi: GLOB.instrument.getItemHeight(nootkaWindow.height)
      height: nootkaWindow.contentItem.height
              - (GLOB.instrument.isSax ? (GLOB.singleNoteMode ? insHi / 7 : 0) : insHi)
              - (examResults ? examResults.height + 2 : 0)
      width: parent.width / (GLOB.singleNoteMode ? 2 : 1)
    }
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
    NOO.mainScore = score
    checkSingleMode()
    if (GLOB.gotIt("soundInfo", true)) {
        sndInf = Qt.createComponent("qrc:/gotit/SoundInfo.qml").createObject(nootkaWindow.contentItem.parent)
        sndInf.remaindChecked = true
        sndInf.closed.connect(function() {
            GLOB.setGotIt("soundInfo", sndInf.remaindChecked)
            sndInf.destroy()
            createStatus() // create status tip only after pitch detection info
        })
    } else
       createStatus()
  }

  function createStatus() {
    Qt.createComponent("qrc:/StatusTip.qml").createObject(NOO.isAndroid() ? nootkaWindow.contentItem : nootkaWindow)
  }

  function checkSingleMode() {
    if (GLOB.singleNoteMode) {
        if (!noteName) {
          var c = Qt.createComponent("qrc:/NoteName.qml")
          noteName = c.createObject(scoreWrap, { "score": score })
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
    if (NOO.isAndroid() && dialogLoader && dialogLoader.visible) {
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
