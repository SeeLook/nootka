/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7


Item {
  width: parent.width
  height: parent.height - (Noo.isAndroid() ? 0 : nootkaWindow.fontSize * 3)

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("global", qsTr("Common"), "Global")
    pages.addItem("scoreSettings", qsTr("Score"), "Score")
    pages.addItem("guitarSettings", qsTr("Instrument"), "Instrument")
    pages.addItem("soundSettings", qsTr("Sound"), "Sound")
    pages.addItem("questionsSettings", qsTr("Exercises") + "\n& " + qsTr("Exam"), "Exam")
    pages.addItem("appearance", qsTr("Appearance"), "Appearance")
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
