/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Slider {
  id: sl

  background: Rectangle {
    x: sl.leftPadding
    y: sl.topPadding + sl.availableHeight / 2 - height / 2
    implicitWidth: Noo.fontSize() * 15; implicitHeight: Noo.fontSize() / 2
    width: sl.availableWidth; height: implicitHeight
    radius: height / 2
    color: sl.enabled ? activPal.mid : disdPal.mid

    Rectangle {
      width: sl.visualPosition * parent.width; height: parent.height
      color:  sl.enabled ? activPal.highlight : disdPal.highlight
      radius: parent.radius
    }
  }

  handle: TipRect {
    scale: Noo.isAndroid() && pressed ? 1.3 : 1
    x: sl.leftPadding + sl.visualPosition * (sl.availableWidth - width / 2)
    y: sl.topPadding + sl.availableHeight / 2 - height / 2
    implicitWidth: Noo.fontSize() * 2; implicitHeight: Noo.fontSize() * 2
    radius: Noo.fontSize()
    rised: !sl.pressed
    color: sl.pressed ? activPal.highlight : activPal.button
    Rectangle {
      anchors.fill: parent; scale: 0.5
      radius: height / 2
      color: activPal.highlightedText
    }
  }
}
