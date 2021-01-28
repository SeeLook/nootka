/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


ListView {
  id: meterList

  property int meters: 0

  signal metersModified()

  clip: true
  width: model * (Noo.factor() * 4.5)
  contentWidth: model * (Noo.factor() * 4.25)
  height: Noo.factor() * 3
  orientation: ListView.Horizontal
  spacing: Noo.factor() / 4

  ScrollBar.horizontal: ScrollBar { active: enabled; visible: active }

  model: 12
  delegate: Component {
    Rectangle {
      width: Noo.factor() * 5; height: Noo.factor() * 3
      color: enabled ? (index % 2 ? activPal.alternateBase : activPal.base) : Qt.darker(disdPal.window, 1.2)
      property int mask: Math.pow(2, index)
      TcheckBox {
        width: Noo.factor() * 5
        x: -y; y: Noo.factor() * 0.2
        checked: meters & mask
        onClicked: {
          if (checked)
            meters |= mask
          else
            meters &= ~mask
          metersModified()
        }
        contentItem: Item {
          height: Noo.factor() * 2; width: Noo.factor() * 2
          Text {
            y: Noo.factor() * -3
            text: Noo.meter(Math.pow(2, index)).symbol()
            leftPadding: Noo.factor() * 2.5
            color: enabled ? activPal.text : disdPal.text
            font { family: "Scorek"; pixelSize: Noo.factor() * 2.2 }
          }
        }
      }
    }
  }
}
