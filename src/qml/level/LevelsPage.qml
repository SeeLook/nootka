/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  id: levelsPage

  height: parent.height; width: parent.width

  spacing: levelsPage.width / 50
  LevelsSelector {
    id: selector
    width: parent.width; height: levelsPage.height - buttRow.height - levelsPage.width / 50 - Noo.fontSize() * 3
    Component.onCompleted: creator.selector = selector
  }
  Row {
    id: buttRow
    spacing: levelsPage.width / 50
    anchors.horizontalCenter: parent.horizontalCenter
    TiconButton {
      enabled: creator.notSaved
      width: levelsPage.width / 4
      pixmap: Noo.pix("notSaved"); iconHeight: levelsPage.height / 15
      text: Noo.TR("QShortcut", "Save")
      onClicked: creator.saveLevel()
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("exam"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exam")
//       onClicked:
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("practice"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exercise")
//       onClicked:
    }
  }
  LinkText {
    text: qsTr("Get more levels <a href=\"%1\">from Nootka home page</a>").arg("https://nootka.sourceforge.io/index.php?C=down#levels")
    anchors.horizontalCenter: parent.horizontalCenter
  }
}
