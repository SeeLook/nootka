/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0


Item {

  Rectangle {
    id: bg
    anchors.fill: parent
    color: activPal.base
    radius: nootkaWindow.fontSize / 4
    visible: false
  }

  DropShadow {
    anchors.fill: bg
    horizontalOffset: nootkaWindow.fontSize / 4
    verticalOffset: nootkaWindow.fontSize / 4
    radius: 8.0
    samples: 17
    color: activPal.shadow
    source: bg
  }
}
