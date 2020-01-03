/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2 as Old

import Nootka 1.0
import Nootka.Dialogs 1.0


Old.Dialog {
  id: dialLoader
  width: Noo.isAndroid() ? Screen.width : nootkaWindow.width * 0.9
  height: Noo.isAndroid() ? Screen.height : nootkaWindow.height * 0.9

  property int page: 0
  property alias standardButtons: box.standardButtons
  property alias buttonBox: box
  property alias dialogObj: dialogObj

  // private
  property var currentDialog: null

  TdialogObject {
    id: dialogObj
    onContinueExam: {
      GLOB.isExam = true
      if (!nootkaWindow.executor.init(1, examFile)) {
        console.log("Executor discarded, deleting it")
        nootkaWindow.executor.destroy()
        GLOB.isExam = false
      }
    }
    onOpenLevel: {
      page = Nootka.LevelCreator
      currentDialog.openLevel(levelFile)
    }
  }

  contentItem: Column {
    width: dialLoader.width; height: dialLoader.height
    Rectangle {
      id: container
      width: parent.width; height: parent.height - (box.visible ? box.height : 0)
      color: activPal.window
    }
    DialogButtonBox {
      id: box
      visible: standardButtons !== 0
      width: parent.width; height: Noo.fontSize() * 3
      padding: Noo.fontSize() / 4
      spacing: Noo.fontSize()
      delegate: TiconButton {
        pixmap: Noo.pix(dialogObj.buttonRoleIcon(DialogButtonBox.buttonRole))
      }
      background: Rectangle {
        anchors.fill: parent
        color: Noo.isAndroid() ? activPal.base : Qt.darker(activPal.window, 1.1)
      }
      onClicked: {
        switch (button.DialogButtonBox.buttonRole) {
          case DialogButtonBox.AcceptRole: dialLoader.accepted(); break
          case DialogButtonBox.HelpRole: dialLoader.help(); break
          case DialogButtonBox.ResetRole: dialLoader.reset(); break
          case DialogButtonBox.ApplyRole: dialLoader.apply(); break
          case DialogButtonBox.ActionRole: break
          default: dialLoader.close()
        }
      }
    }
  }

  onPageChanged: {
    if (page > 0) {
      if (!Noo.isAndroid()) {
        width = nootkaWindow.width * 0.9
        height = nootkaWindow.height * 0.9
      }
      switch (page) {
        case Nootka.Settings:
          var c = Qt.createComponent("qrc:/TsettingsDialog.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.About:
          var c = Qt.createComponent("qrc:/TaboutNootka.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.LevelCreator:
          var c = Qt.createComponent("qrc:/LevelCreator.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.ExamStart:
          var c = Qt.createComponent("qrc:/exam/StartExam.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.ExamSummary:
          var c = Qt.createComponent("qrc:/exam/ExamSummary.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.Tuner:
          var c = Qt.createComponent("qrc:/sound/TunerDialog.qml")
          currentDialog = c.createObject(container)
          break
        case Nootka.MelodyGenerator:
          var c = Qt.createComponent("qrc:/score/MelGenDialog.qml")
          currentDialog = c.createObject(container)
          break
      }
      SOUND.stopListen()
      SOUND.stopPlaying()
      open()
    }
  }

  onVisibleChanged: {
    if (visible === false && currentDialog) {
      if (page !== Nootka.ExamStart)
        SOUND.startListen()
      currentDialog.destroy()
      currentDialog = null
      page = 0
    }
  }

  onApply: { if (currentDialog) currentDialog.apply(); close() }
  onReset: if (currentDialog) currentDialog.reset()
  onAccepted: if (currentDialog) currentDialog.accepted()
  onHelp: if (currentDialog) currentDialog.help()

  function openFile(file) {
    dialogObj.openFile(file)
  }
}
