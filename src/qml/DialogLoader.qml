/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import Nootka 1.0
import Nootka.Dialogs 1.0


Dialog {
  id: dialLoader
  width: Noo.isAndroid() ? Screen.width : nootkaWindow.width * 0.9
  height: Noo.isAndroid() ? Screen.height : nootkaWindow.height * 0.9

  property int page: 0
  // private
  property var currentDialog: null
  property var dialogDrawer: null
  property var buttons: []

  TdialogObject { id: dialogObj }

  onPageChanged: {
    if (page > 0) {
      if (Noo.isAndroid()) {
          standardButtons = 0
      } else {
          width = nootkaWindow.width * 0.9
          height = nootkaWindow.height * 0.9
      }
      switch (page) {
        case Nootka.Settings:
          var c = Qt.createComponent("qrc:/TsettingsDialog.qml")
          currentDialog = c.createObject(contentItem)
          if (Noo.isAndroid()) {
              buttons = [StandardButton.Apply, StandardButton.RestoreDefaults, StandardButton.Help, StandardButton.Cancel]
          } else {
              standardButtons = StandardButton.Apply | StandardButton.Cancel | StandardButton.RestoreDefaults | StandardButton.Help
              title = "Nootka - " + qsTranslate("TsettingsDialog", "application's settings")
          }
          break
        case Nootka.About:
          var c = Qt.createComponent("qrc:/TaboutNootka.qml")
          currentDialog = c.createObject(contentItem)
          if (Noo.isAndroid()) {
            buttons = [StandardButton.Ok]
          } else {
              standardButtons = StandardButton.Ok
              title = qsTranslate("TaboutNootka", "About Nootka")
          }
          break
        case Nootka.LevelCreator:
          var c = Qt.createComponent("qrc:/LevelCreator.qml")
          currentDialog = c.createObject(contentItem)
          if (Noo.isAndroid()) {
              buttons = [StandardButton.Close]
          } else {
              standardButtons = StandardButton.Close
          }
          break
      }
      SOUND.stopListen()
      open()
      if (Noo.isAndroid()) {
          var c = Qt.createComponent("qrc:/+android/DialogDrawer.qml")
          dialogDrawer = c.createObject(currentDialog)
      }
    }
  }


  onVisibleChanged: {
    if (visible === false && currentDialog) {
      SOUND.startListen()
      currentDialog.destroy()
      currentDialog = null
      page = 0
    }
  }

  onApply: {
    if (currentDialog)
      currentDialog.apply()
    close()
  }
  onReset: if (currentDialog) currentDialog.reset()
  onAccepted: if (currentDialog) currentDialog.accepted()
}
