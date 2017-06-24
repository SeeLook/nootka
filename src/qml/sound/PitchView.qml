/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2


Item {
  id: pitchView

  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.25

  IntonationBar {
      width: parent.width
      height: parent.height / 2
  }

  VolumeBar {
      y: parent.height / 2
      width: parent.width
      height: parent.height / 2
  }

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Inactive }
}
