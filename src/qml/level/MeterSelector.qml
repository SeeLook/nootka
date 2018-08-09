/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


ListView {
  id: meterList

//   width: parent.width
  height: Noo.fontSize() * 3
  orientation: ListView.Horizontal
  spacing: Noo.fontSize() / 4
  model: 12
  delegate: Component {
    Rectangle {
      width: Noo.fontSize() * 4; height: Noo.fontSize() * 3
      color: index % 2 ? activPal.alternateBase : activPal.base
      TcheckBox {
        width: Noo.fontSize() * 4
        contentItem: Text {
          height: Noo.fontSize() * 2
          text: Noo.meter(Math.pow(2, index)).symbol()
          topPadding: Noo.fontSize() * -2.2
          leftPadding: Noo.fontSize() * 1.7
          color: activPal.text
          font { family: "Scorek"; pixelSize: Noo.fontSize() * 2 }
        }
      }
    }
  }
}
