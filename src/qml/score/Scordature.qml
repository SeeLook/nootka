/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import Score 1.0


Grid {
  y: upperLine + 14 + (score.clef === Tclef.PianoStaffClefs ? 10 : 0)
  x: 0.5
  spacing: 0.5
  scale: columns > 1 ? 0.75 : (score.clef === Tclef.PianoStaffClefs ? 1.4 : 1.2)
  transformOrigin: Item.TopLeft
  columns: GLOB.tuning.changedStrings() > 3 ? 2 : 1
  visible: score.clef !== Tclef.NoClef
  Repeater {
    model: 6
    Row {
      visible: GLOB.tuning.otherThanStd(index + 1)
      height: 2
      Text {
        text: (index + 1)
        font { pixelSize: 2; family: "Nootka" }
        anchors.verticalCenter: parent.verticalCenter
      }
      Text {
        text: "=" + GLOB.tuning.stringName(index + 1)
        font { pixelSize: 2; family: "Sans" }
        anchors.verticalCenter: parent.verticalCenter
      }
    }
  }
}
