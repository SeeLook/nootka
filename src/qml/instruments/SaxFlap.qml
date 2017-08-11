/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  property int nr: -1
  property bool checked: false
  color: checked ? activPal.text : activPal.base
  x: (parent.width - width) / 2
  width: parent.height / 20
  height: width
  radius: width / 2
  border { width: 2; color: ma.containsMouse ? activPal.highlight : activPal.text }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: checked = !checked
  }
}
