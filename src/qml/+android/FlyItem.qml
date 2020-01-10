/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0


TcuteButton {
  property Taction taction: null

  width: Noo.fingerPixels() * 1.5; height: Noo.fingerPixels() * 1.5
  radius: width / 2

  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on x { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on y { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  Image {
    source: taction ? taction.icon : ""
    height: parent.width * 0.8; width: height * (sourceSize.width / sourceSize.height)
    anchors.centerIn: parent
  }
}
