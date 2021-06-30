/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Column {
      /** @p outShift is the value of selected transposition  */
  property int outShift: keyChB.checked ? NOO.keysDiff(initialKey, selectedKey) : shiftSpin.value * (upRadio.checked ? 1 : -1)
  property bool outScaleToRest: toRestsCombo.currentIndex === 1
  property bool inInstrumentScale: limitCombo.currentIndex === 0
  property alias headerVisible: header.visible
  property int initialKey: 0
  property int selectedKey: transKeyCombo.currentIndex - 7
  property alias toKey: keyChB.checked
  property alias byInterval: semiChB.checked

  // private
  property int shift: 0

  anchors.horizontalCenter: parent.horizontalCenter
  spacing: NOO.factor()
  Text { id: header; text: qsTr("Transpose"); color: activPal.text }
  Row {
    spacing: NOO.factor()
    TcheckBox {
      id: keyChB
      anchors.verticalCenter: parent.verticalCenter
      text: qsTr("to key", "like: Transpose to key")
      onToggled: {
        if (keyChB.checked)
          semiChB.checked = false
        else
          transKeyCombo.currentIndex = 7
      }
    }
    TcomboBox {
      id: transKeyCombo
      anchors.verticalCenter: parent.verticalCenter
      enabled: keyChB.checked
      model: NOO.keyComboModel()
      width: NOO.factor() * 20
      currentIndex: initialKey + 7
    }
  }
  Row {
    spacing: NOO.factor()
    TcheckBox {
      id: semiChB
      text: qsTr("by interval", "like: Transpose by interval")
      anchors.verticalCenter: parent.verticalCenter
      onToggled: {
        if (semiChB.checked)
          keyChB.checked = false
        else
          shift = 0
      }
    }
    Row {
      id: transSemi
      spacing: NOO.factor()
      TspinBox {
        id: shiftSpin
        enabled: semiChB.checked
        anchors.verticalCenter: parent.verticalCenter
        from: 0; to: 12
        value: Math.abs(shift)
        onValueChanged: shift = value * (upRadio.checked ? 1 : -1)
      }
      TlabelText {
        enabled: semiChB.checked
        text: NOO.TR("Transposition", "semitone(s)", "", shiftSpin.value)
        FontMetrics { id: fm }
        width: fm.boundingRect(NOO.TR("Transposition", "semitone(s)", "", 5) + "   ").width
      }
      ButtonGroup { id: upDownGroup }
      Column {
        enabled: semiChB.checked
        onEnabledChanged: {
          upRadio.checked = enabled
          if (!enabled)
            downRadio.checked = false
        }
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
  }
  TcomboBox {
    id: limitCombo
    x: NOO.factor() * 2
    enabled: keyChB.checked || semiChB.checked
    width: NOO.factor() * 20
    model: [ qsTr("Limit notes to instrument scale"), qsTr("Limit notes to notation range") ]
  }
  Row {
    spacing: NOO.factor()
    Text { text: qsTr("Off-scale notes"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
    TcomboBox {
      id: toRestsCombo
      enabled: keyChB.checked || semiChB.checked
      width: NOO.factor() * 20
      model: [ qsTr("raise or drop about octave"), qsTr("convert into rests") ]
    }
  }

}

