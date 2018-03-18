/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Item {
  property alias toolBar: toolBar
  property var scoreMenu: null
  property var melodyMenu: null

  TtoolBar { id: toolBar }

  function open() {
    if (!scoreMenu) {
      var c = Qt.createComponent("qrc:/ScoreMenuContent.qml")
      scoreMenu = c.createObject(nootkaWindow.contentItem)
    }
    scoreMenu.open()
  }

  function melodyOpen() {
    if (!melodyMenu) {
      var c = Qt.createComponent("qrc:/MelodyMenuContent.qml")
      melodyMenu = c.createObject(nootkaWindow.contentItem)
    }
    melodyMenu.open()
  }
}
