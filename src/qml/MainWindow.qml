/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
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

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  Taction {
    id: settingsAct
    icon: "systemsettings"
    text: qsTranslate("TtoolBar", "Settings")
    tip: qsTranslate("TtoolBar", "Application preferences")
    onTriggered: dialogLoader.page = Nootka.Settings
  }
  Taction { id: aboutAct; onTriggered: dialogLoader.page = Nootka.About }
  Taction {
    id: levelAct
    icon: "levelCreator"
    text: qsTranslate("TtoolBar", "Level")
    tip: qsTranslate("TtoolBar", "Levels creator")
    onTriggered: dialogLoader.page = Nootka.LevelCreator
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
//     onTriggered:
  }

  width: GLOB.geometry.width
  height: GLOB.geometry.height
  x: GLOB.geometry.x
  y: GLOB.geometry.y

  onClosing: GLOB.geometry = Qt.rect(x ,y, width, height)

  MainMenu { id: mainMenu }
  header: mainMenu.toolBar

  MainScore {
    id: score
    height: nootkaWindow.height - (header ? header.height : 0) - (GLOB.instrument.isSax ? (GLOB.singleNoteMode ? instrument.height / 7 : 0) : instrument.height)
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
  }

  Component.onCompleted: {
    Noo.mainScore = score
    checkSingleMode()
    SOUND.init()
  }

  function checkSingleMode() {
    if (GLOB.singleNoteMode) {
        if (!noteName) {
          var c = Qt.createComponent("qrc:/NoteName.qml")
          noteName = c.createObject(nootkaWindow.contentItem, { "width": score.width, "height": score.height, "x": score.width + 1, "z": 5 })
          noteName.note = Qt.binding(function() { return score.note })
          noteName.onNoteChanged.connect(function() { score.setNote(score.scoreObj.note(0), noteName.note) })
        }
    } else {
        if (noteName)
          noteName.destroy()
    }
  }

  DialogLoader { id: dialogLoader }
}
