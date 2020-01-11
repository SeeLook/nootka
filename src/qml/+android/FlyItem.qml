/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0


TcuteButton {
  property Taction taction: null
  property bool shaked: false
  property int index: 0
  property bool canShow: taction && taction.enabled

  z: 2000
  x: canShow && (showFlys || shaked) ? flyX(index) : -width
  y: canShow && (showFlys || shaked) ? flyY(index) : -height
  scale: canShow && (showFlys || shaked) ? 0.99 : 0.01
  color: currentFly === this || pressed ? activPal.highlight : activPal.button
  width: Noo.fingerPixels() * 1.5; height: Noo.fingerPixels() * 1.5
  radius: width / 2

  onClicked: {
    taction.trigger()
    shakeTimer.stop()
    shaked = false
  }

  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on x { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on y { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  Image {
    source: taction ? taction.icon : ""
    height: parent.width * 0.7; width: height * (sourceSize.width / sourceSize.height)
    anchors.centerIn: parent
  }

  Connections {
    target: taction
    onShakeButton: {
      shaked = true
      shakeTimer.start()
    }
  }

  Timer {
    id: shakeTimer
    interval: 2500
    onTriggered: shaked = false
  }
}
