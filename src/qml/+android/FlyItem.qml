/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0


TcuteButton {
  id: flyItem
  property Taction taction: null
  property bool shaked: false
  property int index: 0
  property bool canShow: taction && taction.enabled

  x: canShow && (showFlys || shaked) ? flyX(index) : -width
  y: canShow && (showFlys || shaked) ? flyY(index) : -height
  scale: canShow && (showFlys || shaked) ? 0.99 : 0.01
  color: Qt.tint(activPal.base, Noo.alpha(taction ? taction.bgColor : activPal.base, currentFly === this || pressed ? 200 : 50))
  border { width: 2; color: taction ? taction.bgColor : "transparent" }
  width: Noo.fingerPixels() * 1.5 + (txt.visible ? txt.width : 0)
  height: Noo.fingerPixels() * 1.5
  radius: width / 2

  onClicked: {
    taction.trigger()
    shakeTimer.stop()
    shaked = false
  }

  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on x { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on y { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  contentItem: Item {
    Image {
      id: img
      x: (Noo.fingerPixels() * 1.5 - width) / 2
      source: taction ? taction.icon : ""
      height: parent.height * 0.7; width: height * (sourceSize.width / sourceSize.height)
      anchors.verticalCenter: parent.verticalCenter
    }
    Text {
      id: txt
      visible: forceText || (showText && taction && currentFly === flyItem)
      anchors { verticalCenter: parent.verticalCenter; left: img.right }
      padding: Noo.fontSize() / 3
      horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
      minimumPixelSize: 8
      fontSizeMode: Text.HorizontalFit
      color: activPal.text
      text: taction ? taction.text : ""
    }
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
    onTriggered: {
      if (pressed)
        shakeTimer.start()
      else
        shaked = false
    }
  }
}
