/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Row {
  property int shift: 0
  property int outShift: shiftSpin.value * (upRadio.checked ? 1 : -1) // read shift from here

  onShiftChanged: {
    upRadio.checked = shift >= 0
    downRadio.checked = shift < 0
  }

  anchors.horizontalCenter: parent.horizontalCenter
  spacing: nootkaWindow.fontSize

  Text { text: qsTr("Transposition"); anchors.verticalCenter: parent.verticalCenter }
  SpinBox {
    id: shiftSpin
    anchors.verticalCenter: parent.verticalCenter
    from: 0; to: 24
    value: Math.abs(shift)
    onValueChanged: shift = value * (upRadio.checked ? 1 : -1)
  }
  Text { text: qsTr("semitone(s)", "", shiftSpin.value); width: nootkaWindow.fontSize * 15; anchors.verticalCenter: parent.verticalCenter }
  Column {
    RadioButton { id: upRadio; text: "<b><font size=\"7\">\u2191</font></b> " + qsTr("up") }
    RadioButton { id: downRadio; text: "<b><font size=\"7\">\u2193</font></b> " + qsTr("down") }
  }
}