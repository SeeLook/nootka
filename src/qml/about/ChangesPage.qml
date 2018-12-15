/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


import "../"

Tflickable {
  id: flick
  contentHeight: text.paintedHeight

  Rectangle {
      anchors.fill: parent
      color: activPal.base
  }

  TextEdit {
      id: text
      width: flick.width
      textMargin: font.pixelSize
      wrapMode: TextEdit.Wrap
      readOnly: true
      textFormat: TextEdit.RichText
      color: activPal.text
      text: dialogObj.getChanges()
  }
}

