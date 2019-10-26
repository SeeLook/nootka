/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tile {
  property alias diversity: rtmDivSpin.value

  signal diversityModified()

  description: qsTr("Smaller values - less number of selected rhythmic note groups will be used, biggest value - melody will consist maximal possible number of selected groups.")
  Row {
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: Noo.fontSize()
    Text {
      anchors.verticalCenter: parent.verticalCenter
      text: qsTr("Rhythmic diversity")
      color: enabled ? activPal.text : disdPal.text
    }
    TspinBox {
      id: rtmDivSpin
      from: 1; to: 10
      onValueModified: diversityModified()
    }
  }
}
