/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12


Grid {
  property int shift: 0
  property int outShift: shiftSpin.value * (upRadio.checked ? 1 : -1) // read shift from here

  onShiftChanged: {
    upRadio.checked = shift >= 0
    downRadio.checked = shift < 0
  }

  anchors.horizontalCenter: parent.horizontalCenter
  horizontalItemAlignment: Grid.AlignHCenter; verticalItemAlignment: Grid.AlignVCenter
  spacing: NOO.factor()
  columns: parent.width < NOO.factor() * 50 ? 1 : 2

  Row {
    spacing: NOO.factor()
    TlabelText { text: qsTr("Transposition") }
    TspinBox {
      id: shiftSpin
      anchors.verticalCenter: parent.verticalCenter
      from: 0; to: 24
      value: Math.abs(shift)
      onValueChanged: shift = value * (upRadio.checked ? 1 : -1)
    }
    TlabelText {
      text: qsTr("semitone(s)", "", shiftSpin.value)
      FontMetrics { id: fm }
      width: fm.boundingRect(NOO.TR("Transposition", "semitone(s)", "", 5) + "   ").width
    }
  }

  ButtonGroup { id: upDownGroup }
  Column {
    Row {
      TlabelText { text: "\u2191"; font { pixelSize: NOO.factor() * 2.5; family: "Nootka" }}
      TradioButton { id: upRadio; text: qsTr("up"); ButtonGroup.group: upDownGroup }
    }
    Row {
      TlabelText { text: "\u2193"; font { pixelSize: NOO.factor() * 2.5; family: "Nootka" }}
      TradioButton { id: downRadio; text: qsTr("down"); ButtonGroup.group: upDownGroup }
    }
  }
}
