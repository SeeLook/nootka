/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2 as Old

import Nootka 1.0
import Nootka.Dialogs 1.0


Old.Dialog {
  id: dialLoader
  width: nootkaWindow.width * (NOO.isAndroid() ? 1 : 0.9)
  height: nootkaWindow.height * (NOO.isAndroid() ? 1 : 0.9)

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
      nootkaWindow.executor.init(1, examFile)
      close()
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
      width: parent.width; height: NOO.factor() * 3
      spacing: NOO.factor()
      alignment: Qt.AlignVCenter
      delegate: TiconButton {
        width: Math.min(box.width / 2, (box.width - NOO.factor() * (box.count + 1)) / box.count)
        pixmap: NOO.pix(dialogObj.buttonRoleIcon(DialogButtonBox.buttonRole))
      }
      background: Rectangle {
        anchors.fill: parent
        color: NOO.isAndroid() ? activPal.base : Qt.darker(activPal.window, 1.1)
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
      if (NOO.isAndroid())
        mainMenu.drawer.interactive = false

      dialLoader.width = nootkaWindow.width * (NOO.isAndroid() ? 1 : 0.9)
      dialLoader.height = nootkaWindow.height * (NOO.isAndroid() ? 1 : 0.9)

      open() // do it it first, to initialize size at first time

      switch (page) {
        case Nootka.Settings:
          currentDialog = Qt.createComponent("qrc:/TsettingsDialog.qml").createObject(container)
          break
        case Nootka.About:
          currentDialog = Qt.createComponent("qrc:/TaboutNootka.qml").createObject(container)
          break
        case Nootka.LevelCreator:
          currentDialog = Qt.createComponent("qrc:/LevelCreator.qml").createObject(container)
          break
        case Nootka.ExamStart:
          currentDialog = Qt.createComponent("qrc:/exam/StartExam.qml").createObject(container)
          break
        case Nootka.ExamSummary:
          currentDialog = Qt.createComponent("qrc:/exam/ExamSummary.qml").createObject(container)
          break
        case Nootka.Tuner:
          currentDialog = Qt.createComponent("qrc:/sound/TunerDialog.qml").createObject(container)
          break
        case Nootka.MelodyGenerator:
          currentDialog = Qt.createComponent("qrc:/score/MelGenDialog.qml").createObject(container)
          break
      }
      SOUND.stopListen()
      SOUND.stopPlaying()
    }
  }

  onVisibleChanged: {
    if (visible === false && currentDialog) {
      if (NOO.isAndroid())
        mainMenu.drawer.interactive = true

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
