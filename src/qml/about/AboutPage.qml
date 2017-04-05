/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7


Item {

  width: parent.width
  height: parent.height

  Column {
    anchors.fill: parent
    Rectangle {
      height: parent.height / 7
      width: parent.width
      color: nooLab.bgColor
      NootkaLabel {
        id: nooLab
        height: parent.height
        active: false
        anchors.centerIn: parent
        bgColor: randColor()
      }
    }
  }
}
