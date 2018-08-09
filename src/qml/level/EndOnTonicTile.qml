/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Tile {
  property alias checkBox: tonicChB
  property alias checked: tonicChB.checked

  anchors.horizontalCenter: undefined
  width: tonicChB.width * 1.2
  description: qsTr("Determines the last note of a melody.<br>When set, melody will be finished on tonic note in actual key signature.").replace("<br>", " ")
  TcheckBox {
    id: tonicChB
    anchors.horizontalCenter: parent.horizontalCenter
    text: qsTr("Melody ends on tonic note")
  }
}

