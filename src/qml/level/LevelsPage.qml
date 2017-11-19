/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"

Tflickable {
  id: levelsPage

  height: parent.height
  contentWidth: width
  contentHeight:levelsCol.height

  Column {
    id: levelsCol
    anchors.fill: parent
    Row {
      width: parent.width
      Rectangle {
        width: parent.width / 2; height: levelsPage.height * 0.6
        color: "gray"
      }
      LevelPreview {
        width: parent.width / 2; height: levelsPage.height * 0.7
      }
    }
  }
}
