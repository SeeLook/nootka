/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Nootka 1.0


Item {
  property alias note: piano.note

  anchors.fill: parent

  TpianoBg {
    id: piano
    anchors.fill: parent
    keyWidth: nootkaWindow.fontSize * 2
    onNoteChanged: {
      selectedKey.x = keyRect.x
      selectedKey.y = keyWidth + keyRect.height - height * 0.3
      selectedKey.width = keyRect.width
    }
  }

  Rectangle {
    id: selectedKey
    color: GLOB.selectedColor
    height: parent.height * 0.3
    radius: width * 0.2
    visible: x > 0
  }

  Rectangle {
    color: GLOB.fingerColor
    width: piano.keyRect.width
    height: piano.keyRect.height
    radius: width * 0.2
    x: piano.keyRect.x
    y: piano.keyRect.y
    visible: piano.active
  }
}
