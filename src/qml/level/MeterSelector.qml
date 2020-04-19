/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


ListView {
  id: meterList

  property int meters: 0

  signal metersModified()

  clip: true
  width: model * (Noo.fontSize() * 4.5)
  contentWidth: model * (Noo.fontSize() * 4.25)
  height: Noo.fontSize() * 3
  orientation: ListView.Horizontal
  spacing: Noo.fontSize() / 4

  ScrollBar.horizontal: ScrollBar { active: enabled; visible: active }

  model: 12
  delegate: Component {
    Rectangle {
      width: Noo.fontSize() * 5; height: Noo.fontSize() * 3
      color: enabled ? (index % 2 ? activPal.alternateBase : activPal.base) : disdPal.base
      property int mask: Math.pow(2, index)
      TcheckBox {
        width: Noo.fontSize() * 5
        x: -y; y: Noo.fontSize() * 0.2
        checked: meters & mask
        onClicked: {
          if (checked)
            meters |= mask
          else
            meters &= ~mask
          metersModified()
        }
        contentItem: Item {
          height: Noo.fontSize() * 2; width: Noo.fontSize() * 2
          Text {
            y: Noo.fontSize() * -3
            text: Noo.meter(Math.pow(2, index)).symbol()
            leftPadding: Noo.fontSize() * 2.5
            color: enabled ? activPal.text : disdPal.text
            font { family: "Scorek"; pixelSize: Noo.fontSize() * 2.2 }
          }
        }
      }
    }
  }
}
