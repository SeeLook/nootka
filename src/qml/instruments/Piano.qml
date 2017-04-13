/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Window 2.0

import Nootka 1.0


Item {
  property alias note: piano.note

  anchors.fill: parent

  TpianoBg {
    id: piano
    anchors.fill: parent
    keyWidth: Screen.pixelDensity * 5.5
    onNoteChanged: {
      selectedKey.width = keyRect.width
      selectedKey.height = keyRect.height
      selectedKey.x = keyRect.x
      selectedKey.y = keyRect.y
    }
  }

  Rectangle {
    id: selectedKey
    color: Qt.rgba(0.2, 0.6, 1.0, 1.0)
    radius: width * 0.2
  }

  Rectangle {
    color: Qt.rgba(1, 0, 0.5, 0.78)
    width: piano.keyRect.width
    height: piano.keyRect.height
    radius: width * 0.2
    x: piano.keyRect.x
    y: piano.keyRect.y
    visible: piano.active
  }
}
