/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


ExamTip {
  id: examTip

  bg: GLOB.wrongColor
  font.pixelSize: Noo.fontSize() * 1.25
  width: textItem.width + height / 2

  MouseArea {
    id: dragArea
    anchors.fill: parent
    drag.target: parent
    drag.minimumX: shadowRadius; drag.maximumX: executor.width - width * scale - shadowRadius
    drag.minimumY: shadowRadius; drag.maximumY: executor.height - height * scale - shadowRadius
  }

  Text {
    z: 205
    font { pixelSize: examTip.height; family: "Nootka" }
    x: examTip.width - width + height * 0.15
    text: "?"; color: Qt.tint(activPal.base, Noo.alpha(GLOB.wrongColor, 70))
  }

  Drag.active: dragArea.drag.active

  Component.onCompleted: {
    if (GLOB.useAnimations)
      rotation = 360
  }
  Component.onDestruction: {
    if (tipHandler)
      tipHandler.tipPos = Qt.point((x + width / 2) / scale, (y + height / 2) / scale)
  }

  Behavior on scale { NumberAnimation { duration: 300 }}
  Behavior on rotation { NumberAnimation { duration: 300 }}
}
