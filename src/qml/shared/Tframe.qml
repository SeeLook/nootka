/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


/**
 * Common frame with custom background and palette-aware colors
 */
Frame {
  property alias border: bg.border
  property alias bgColor: bg.color
  background: Rectangle {
    id: bg
    color: "transparent"
    border.color: enabled ? activPal.text : disdPal.text
    radius: Noo.fontSize() / 2
  }
}
