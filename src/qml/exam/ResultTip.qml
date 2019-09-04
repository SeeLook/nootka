/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0


Item {
  id: resultTip

  property real targetY: executor.height / 80
  property alias color: txt.color
  property alias text: txt.text

  anchors { right: parent.right; rightMargin: parent.width / 12 }

  width: txt.width; height: txt.height
  z: 501 // above status tip rectangle

  Text {
    id: txt
    font { pixelSize: (executor.height / 15) * (1 - 0.2 * (lineCount - 1)); family: "Sans"; bold: true }
    visible: false
    horizontalAlignment: Text.AlignHCenter; textFormat: Text.StyledText
  }

  DropShadow {
    anchors.fill: txt
    horizontalOffset: txt.font.pixelSize / 12
    verticalOffset: horizontalOffset
    radius: executor.height / 100
    samples: radius * 2 + 1
    color: activPal.shadow
    source: txt
  }

  transformOrigin: Item.Top

  Component.onCompleted: {
    if (GLOB.useAnimations)
      anim.running = true
    y = GLOB.useAnimations ? -2 * height : targetY
  }

  SequentialAnimation {
    id: anim
    NumberAnimation { target: resultTip; property: "y"; to: targetY; duration: 200 }
    NumberAnimation { target: resultTip; property: "scale"; to: 2; duration: 200 }
    NumberAnimation { target: resultTip; property: "scale"; to: 1; duration: 300 }
  }
}
