/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0


Item {
  property alias color: bg.color
  property alias radius: bg.radius
  property bool rised: true
  property real shadowRadius: 0
  property color shadowColor: "transparent"
  property alias border: bg.border
  property int horizontalOffset: 0
  property int verticalOffset: 0

  BorderImage {
    x: -6; y: -6; width: parent.width + 12; height: parent.height + 12
    border { left: 12; right: 12; bottom: 12; top: 12 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
    source: Noo.pix("tipbg")

    Rectangle {
      id: bg
      x: 6; y: 6; width: parent.width - 12; height: parent.height - 12
      color: activPal.base
      radius: 6
      clip: true
    }

  }
}
