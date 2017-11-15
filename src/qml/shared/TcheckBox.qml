/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


CheckBox {
  id: chB
  property alias textColor: content.color

  indicator: Rectangle {
    implicitWidth: Noo.fontSize() * 1.3
    implicitHeight: Noo.fontSize() * 1.3
    x: chB.leftPadding
    y: chB.height / 2 - height / 2
    radius: Noo.fontSize() * 0.1
    border.color: chB.enabled ? (chB.down ? activPal.highlight : activPal.dark) : disdPal.text
    color: chB.enabled ? activPal.base : disdPal.base

    Rectangle {
      width: Noo.fontSize() * 0.8
      height: Noo.fontSize() * 0.8
      x: Noo.fontSize() * 0.25
      y: Noo.fontSize() * 0.25
      radius: Noo.fontSize() * 0.1
      color: chB.enabled ? (chB.down ? activPal.highlight : activPal.dark) : disdPal.text
      visible: chB.checked
    }
  }

  contentItem: Text {
    id: content
    text: chB.text
    y: (chB.height - height) / 2
    leftPadding: indicator.width + font.pixelSize / 2
    color: chB.enabled ? activPal.text : disdPal.text
    font { family: chB.font.family; pixelSize: Noo.fontSize() }
  }
}
