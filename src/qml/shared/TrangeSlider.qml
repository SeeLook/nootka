/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12


RangeSlider {
  id: sl

  background: Rectangle {
    x: sl.leftPadding
    y: sl.topPadding + sl.availableHeight / 2 - height / 2
    implicitWidth: NOO.factor() * 15; implicitHeight: NOO.factor() / 2
    width: sl.availableWidth; height: implicitHeight
    radius: height / 2
    color: sl.enabled ? activPal.mid : disdPal.mid
    border { width: sl.activeFocus ? 1 : 0; color: activPal.highlight }

    Rectangle {
      width: sl.visualPosition * parent.width; height: parent.height
      color:  sl.enabled ? activPal.highlight : disdPal.highlight
      radius: parent.radius
    }
  }

  first.handle: TipRect {
    scale: NOO.isAndroid() && first.pressed ? 1.3 : 1
    x: sl.leftPadding + sl.first.visualPosition * (sl.availableWidth - width / 2)
    y: sl.topPadding + sl.availableHeight / 2 - height / 2
    implicitWidth: NOO.factor() * 2; implicitHeight: NOO.factor() * 2
    radius: NOO.factor()
    rised: !sl.first.pressed
    color: sl.first.pressed ? activPal.highlight : activPal.button
    Rectangle {
      anchors.fill: parent; scale: 0.5
      radius: height / 2
      color: sl.activeFocus ? activPal.text : activPal.highlightedText
      border { width: sl.activeFocus ? 1 : 0; color: activPal.highlight }
    }
  }

  second.handle: TipRect {
    scale: NOO.isAndroid() && second.pressed ? 1.3 : 1
    x: sl.leftPadding + sl.second.visualPosition * (sl.availableWidth - width / 2)
    y: sl.topPadding + sl.availableHeight / 2 - height / 2
    implicitWidth: NOO.factor() * 2; implicitHeight: NOO.factor() * 2
    radius: NOO.factor()
    rised: !sl.second.pressed
    color: sl.second.pressed ? activPal.highlight : activPal.button
    Rectangle {
      anchors.fill: parent; scale: 0.5
      radius: height / 2
      color: sl.activeFocus ? activPal.text : activPal.highlightedText
      border { width: sl.activeFocus ? 1 : 0; color: activPal.highlight }
    }
  }
}

