/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  id: levelsPage

  height: parent.height; width: parent.width

  spacing: levelsPage.width / 50
  LevelsSelector {
    id: selector
    width: parent.width
    height: levelsPage.height - buttRow.height - levelsPage.width / 25 - (Noo.isAndroid() ? 0 : Noo.fontSize() * 3)
    Component.onCompleted: creator.selector = selector
  }
  Row {
    id: buttRow
    spacing: levelsPage.width / 50
    anchors.horizontalCenter: parent.horizontalCenter
    TiconButton {
      enabled: creator.notSaved
      width: levelsPage.width / 4
      pixmap: Noo.pix("pane/notSaved"); iconHeight: levelsPage.height / 15
      text: Noo.TR("QShortcut", "Save")
      onClicked: creator.saveLevel()
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("exam"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exam")
      onClicked: start(2, selector.currentLevelVar())
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("practice"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exercise")
      onClicked: start(4, selector.currentLevelVar())
    }
  }
  LinkText {
    visible: !Noo.isAndroid()
    text: qsTr("Get more levels <a href=\"%1\">from Nootka home page</a>").arg("https://nootka.sourceforge.io/index.php/download/#levels")
    anchors.horizontalCenter: parent.horizontalCenter
  }

  function start(action, argument) {
    GLOB.isExam = true
    if (!nootkaWindow.executor.init(action, argument)) {
        console.log("Executor discarded, deleting it")
        nootkaWindow.executor.destroy()
        GLOB.isExam = false
    }
    dialLoader.close()
  }
}
