/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import ".."


Popup {
  property alias tickEnable: meterTickChB.checked
  property alias count: countChB.checked

  margins: Noo.fontSize()

  background: TipRect { color: activPal.window; shadowRadius: Noo.fontSize() }

  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0.0 }}

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

    TiconButton {
      width: parent.width - Noo.fontSize()
      text: qsTr("Tap tempo")
      pixmap: Noo.pix("fingerpoint")
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: tapTempo()
    }

    ButtonGroup { buttons: radioRow.children }

    Row {
      id: radioRow
      spacing: Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter
      Text { text: qsTr("round to:"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
      RadioButton {
        id: radio16
        font { family: "Nootka"; pixelSize: Noo.fontSize() * 2 }
        text: "G"
        checked: SOUND.quantization === 6
      }
      RadioButton {
        id: radio8
        font { family: "Nootka"; pixelSize: Noo.fontSize() * 2 }
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

    TiconButton {
      text: Noo.TR("QPlatformTheme", "Apply")
      pixmap: Noo.pix("check")
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

  onOpened: { SOUND.stopListen(); spaceShort.enabled = true } 
  onClosed: { SOUND.startListen(); spaceShort.enabled = false } 

  Shortcut { id: spaceShort; sequence: " "; onActivated: tapTempo() }

  property real lastTime: new Date().getTime()

  function tapTempo() {
    var currTime = new Date().getTime()
    if (currTime - lastTime < 1500)
      tempoSpin.value = Math.round(60000 / (currTime - lastTime))
    lastTime = currTime
  }
}
