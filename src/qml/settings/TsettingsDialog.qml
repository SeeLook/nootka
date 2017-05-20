/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import "settings"


Item {
  width: parent.width
  height: parent.height - (Noo.isAndroid() ? 0 : nootkaWindow.fontSize * 3)

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("global", qsTr("Common"), "settings/Global")
    pages.addItem("scoreSett", qsTr("Score"), "settings/Score")
    pages.addItem("instrSett", qsTr("Instrument"), "settings/Instrument")
    pages.addItem("soundSett", qsTr("Sound"), "settings/Sound")
    pages.addItem("questionsSett", qsTr("Exercises") + "\n& " + qsTr("Exam"), "settings/Exam")
    pages.addItem("appearance", qsTr("Appearance"), "settings/Appearance")
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

  function reset() {
    pages.currentPage.defaults()
  }
}
