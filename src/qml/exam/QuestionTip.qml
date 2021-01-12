/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


ExamTip {
  id: examTip

  bg: GLOB.wrongColor
  font.pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 1 : 1.25)
  width: textItem.width + height / 2
  textItem.z: 1 // above question mark

  Text {
    font { pixelSize: examTip.height; family: "Nootka" }
    x: examTip.width - width * 0.95
    text: "?"; color: Qt.tint(activPal.base, Noo.alpha(GLOB.wrongColor, 70))
  }

  MouseArea {
    id: dragArea
    anchors.fill: parent
    z: 2 // above all context
    hoverEnabled: true
    drag.target: parent
    drag.smoothed: false
    drag.minimumX: shadowRadius; drag.maximumX: executor.width - width * scale - shadowRadius
    drag.minimumY: shadowRadius; drag.maximumY: executor.height - height * scale - shadowRadius
    cursorShape: drag.active ? Qt.DragMoveCursor : (containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor)
  }

  Component.onCompleted: {
    if (GLOB.useAnimations)
      rotation = 360
  }

  Behavior on scale { NumberAnimation { duration: 300 }}
  Behavior on rotation { NumberAnimation { duration: 300 }}
}
