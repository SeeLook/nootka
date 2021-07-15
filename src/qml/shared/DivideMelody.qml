/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Column {
  property alias divisionBy: splitSpin.value

  spacing: NOO.factor()
  Row {
    id: splitRow
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: NOO.factor()
    Text {
      anchors.verticalCenter: parent.verticalCenter
      color: activPal.text; text: qsTr("Divide by selected bars number")
    }
    TspinBox {
      id: splitSpin
      from: 0; to: 64
    }
  }
}
