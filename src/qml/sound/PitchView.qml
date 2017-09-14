/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2

import "sound"


Item {
  id: pitchView

  property alias volume : volBar.volume
//   property alias deviation : intoBar.deviation
  property bool active: false

  signal paused()

  antialiasing: true

  // protected
  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.4

//   IntonationBar {
//       id: intoBar
//       y: parent.height * 0.05
//       width: parent.width
//       height: parent.height * 0.45
//   }

  TempoBar {
    id: tempoBar
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
