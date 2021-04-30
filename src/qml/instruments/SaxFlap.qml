/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  property int nr: -1
  property bool checked: fingeringId & Math.pow(2, nr)

  // private
  property bool marked: checked && parent.markColor.a

  color: enabled && ma.containsMouse ? GLOB.fingerColor : (checked ? checkedColor : activPal.base)
  x: (parent.width - width) / 2
  width: parent.height / 20
  height: width
  radius: width / 2
  border { width: (marked ? 4 : 2); color: marked ? parent.markColor : activPal.text }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: flapNumber = nr
  }
}
