/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2


Item {
  id: pitchView

  property alias volume : volBar.volume
  property alias minVol : volBar.minVol
  property alias deviation : intoBar.deviation

  // private
  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.25

  IntonationBar {
      id: intoBar
      y: parent.height * 0.05
      width: parent.width
      height: parent.height * 0.45
  }

  VolumeBar {
      id: volBar
      y: parent.height * 0.55
      width: parent.width
      height: parent.height * 0.45
  }

}
