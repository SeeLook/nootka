/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import ".."


Popup {
  property alias tickEnable: meterTickChB.checked
  property alias count: countChB.checked

  margins: Noo.fontSize()

  signal accepted()

  Column {
    spacing: Noo.fontSize() / 2

    Row {
      spacing: Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter
      Text { text: qsTr("tempo"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
      SpinBox {
        id: tempoSpin
        from: 40; to: 180; editable: true
        value: SOUND.tempo
      }
    }
    Slider {
      width: parent.width * 0.96
      anchors.horizontalCenter: parent.horizontalCenter
      value: tempoSpin.value
      from: 40; to: 180;
      onValueChanged: tempoSpin.value = value
      stepSize: 10
    }

    ButtonGroup { buttons: radioRow.children }

    Row {
      id: radioRow
      spacing: Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter
      Text { text: qsTr("round to:"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
      RadioButton {
        id: radio16
        font { family: "Nootka"; pixelSize: Noo.fontSize() * 2.5 }
        text: "G"
        checked: SOUND.quantization === 6
      }
      RadioButton {
        id: radio8
        font { family: "Nootka"; pixelSize: Noo.fontSize() * 2.5 }
        text: "F"
        checked: SOUND.quantization === 12
      }
    }

    TcheckBox {
      id: meterTickChB
      text: qsTr("Enable metronome ticking")
      checked: true
    }

    TcheckBox {
      id: countChB
      text: qsTr("Count up")
      checked: true
    }

    Button {
      text:  Noo.TR("QPlatformTheme", "Apply")
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
        SOUND.tempo = tempoSpin.value
        SOUND.quantization = radio16.checked ? 6 : 12 // See Tsound doc for values explanation
        tempoSpin.value = SOUND.tempo
        accepted()
        close()
      }
    }
  }

  onOpened: SOUND.stopListen()
  onClosed: SOUND.startListen()
}
