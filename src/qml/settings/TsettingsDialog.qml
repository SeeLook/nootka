/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "settings"


Item {
  width: parent.width
  height: parent.height

  PagesDialog { id: pages }

  Component.onCompleted: {
    if (!GLOB.isExam) {
      pages.addItem("global", qsTr("Common"), "settings/Global")
      pages.addItem("score", qsTr("Score"), "settings/Score")
      pages.addItem("instrument", qsTr("Instrument"), "settings/Instrument")
      pages.addItem("sound", qsTr("Sound"), "settings/Sound")
    }
    pages.addItem("questions", qsTr("Exercises") + "\n& " + qsTr("Exam"), "settings/Exam")
    if (Noo.isAndroid()) {
        dialLoader.buttons = [DialogButtonBox.Apply, DialogButtonBox.RestoreDefaults, DialogButtonBox.Help, DialogButtonBox.Cancel]
    } else {
        dialLoader.standardButtons = DialogButtonBox.Apply | DialogButtonBox.Cancel | DialogButtonBox.RestoreDefaults | DialogButtonBox.Help
        dialLoader.title = "Nootka - " + qsTranslate("TsettingsDialog", "application's settings")
    }
  }

  Connections {
    target: pages.mobileButt
    onClicked: dialLoader.dialogDrawer.open()
  }

  function apply() {
    for (var i = 0; i < pages.pages.length; ++i) {
      if (typeof(pages.pages[i]) === 'object')
        pages.pages[i].save()
    }
  }

  function reset() { pages.currentPage.defaults() }

  function help() { pages.currentPage.help() }
}
