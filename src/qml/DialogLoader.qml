/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

import Nootka 1.0


Dialog {
  id: dialLoader
  width: Noo.isAndroid() ? Screen.width : nootkaWindow.width * 0.75
  height: Noo.isAndroid() ? Screen.height : nootkaWindow.height * 0.75

  property int page: 0
  // private
  property var currentDialog: null
  property Drawer dialogDrawer: null

  onPageChanged: {
    if (page > 0) {
      if (Noo.isAndroid()) {
          standardButtons = 0
      } else {
          width = nootkaWindow.width * 0.75
          height = nootkaWindow.height * 0.75
      }
      switch (page) {
        case Nootka.Settings:
          var c = Qt.createComponent("qrc:/TsettingsDialog.qml")
          currentDialog = c.createObject(contentItem)
          if (!Noo.isAndroid()) {
            standardButtons = StandardButton.Apply | StandardButton.Cancel | StandardButton.RestoreDefaults | StandardButton.Help
            title = "Nootka - " + qsTranslate("TsettingsDialog", "application's settings")
          }
          break
        case Nootka.About:
          var c = Qt.createComponent("qrc:/TaboutNootka.qml")
          currentDialog = c.createObject(contentItem)
          if (!Noo.isAndroid()) {
            standardButtons = StandardButton.Ok
            title = qsTranslate("TaboutNootka", "About Nootka")
          }
          break
      }
      open()
      if (Noo.isAndroid()) {
        dialogDrawer = drawerComp.createObject(currentDialog)
      }
    }
  }

  Component {
    id: drawerComp
    Drawer {
      width: nootkaWindow.fontSize * 15
      height: parent.height
      Column {
      }
    }
  }

  onVisibleChanged: {
    if (visible === false && currentDialog) {
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
