/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Column {
  spacing: Noo.fontSize()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width * 0.9

  Text {
    text: "introduce piano..."
    color: activPal.text
  }
}
