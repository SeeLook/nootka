/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Tile {
  property alias checkBox: currKeyChB
  property alias checked: currKeyChB.checked

  TcheckBox {
    id: currKeyChB
    text: qsTranslate("AccidsPage", "notes in current key signature only")
    anchors.horizontalCenter: parent.horizontalCenter
  }
}
