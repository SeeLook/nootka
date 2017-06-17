/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7


TipRect {
  id: base
  property bool active: false
  property real factor: score.scale * 2
  property alias component: loader.sourceComponent
  property alias hideTimer: hideTimer

  // protected
  property bool show: false

  z: 20
  width: loader.item ? loader.item.width : 0
  height: loader.item ?loader.item.height : 0

  Loader {
    id: loader
    active: false
  }

  onActiveChanged: {
    if (active) {
        loader.active = true
        show = true
        hideTimer.stop()
    } else
        hideTimer.restart()
  }

  Timer {
    id: hideTimer
    interval: 1000
    repeat: false
    onTriggered: show = false
  }
}
