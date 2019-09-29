/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  id: flick
  contentHeight: text.paintedHeight; contentWidth: flick.width

  Rectangle {
    anchors.fill: parent
    color: activPal.base
  }

  TextEdit {
    id: text
    textMargin: font.pixelSize
    width: flick.width
    wrapMode: TextEdit.Wrap
    readOnly: true
    color: activPal.text
    text: dialogObj.getLicense()
  }
}
