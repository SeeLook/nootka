/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import ".."


Popup {
  property alias enableMetronome: metroVisibleChB.checked
  property alias count: countChB.checked

  margins: Noo.fontSize()

  background: TipRect { color: activPal.window; shadowRadius: Noo.fontSize() }

  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0.0 }}

  signal accepted()

  // private
  property var beatFactor: [ 1, 2, 0.75, 0.5 ]

  Column {
    spacing: Noo.fontSize() / 2

    Row {
      spacing: Noo.fontSize()
      Tumbler {
        id: beatUnitTumb
        background: Rectangle { color: activPal.base }
        anchors.verticalCenter: parent.verticalCenter
        height: Noo.fontSize() * 6; width: Noo.fontSize() * 2
        model: tempoBar.beatModel
        visibleItemCount: 3; wrap: true
        currentIndex: SOUND.beatUnit
        delegate: Text {
          text: modelData
          color: activPal.text
          height: Noo.fontSize() * 2.5
          horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
          opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
          font { pixelSize: Noo.fontSize() * 2; family: "Scorek" }
          MouseArea {
            id: tumblerArea
            anchors.fill: parent
            onClicked: {
//               var prevBeatFactor = beatFactor[beatUnitTumb.currentIndex]
              beatUnitTumb.currentIndex = beatUnitTumb.currentIndex === beatUnitTumb.count - 1 ? beatUnitTumb.currentIndex = 0 : beatUnitTumb.currentIndex + 1
//               tempoSpin.value /= prevBeatFactor / beatFactor[beatUnitTumb.currentIndex]
            }
          }
        }
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: "="
        color: activPal.text
        font { pixelSize: Noo.fontSize() * 2 }
      }
      Column {
        anchors.verticalCenter: parent.verticalCenter
        Row {
          spacing: Noo.fontSize()
          anchors.horizontalCenter: parent.horizontalCenter
          Text { text: qsTr("tempo"); font.pixelSize: Noo.fontSize(); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
          SpinBox {
            id: tempoSpin
            from: 40; to: 180 * beatFactor[beatUnitTumb.currentIndex]; editable: true
            value: SOUND.tempo
          }
        }
        Slider {
          width: parent.width * 0.96
          anchors.horizontalCenter: parent.horizontalCenter
          value: tempoSpin.value
          from: 40; to: 180 * beatFactor[beatUnitTumb.currentIndex]
          onValueChanged: tempoSpin.value = value
          stepSize: 10
        }
      }
    }

    TiconButton {
      width: parent.width - Noo.fontSize()
      text: qsTr("Tap tempo")
      pixmap: Noo.pix("fingerpoint")
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: tapTempo()
    }

    TcheckBox {
      id: metroVisibleChB
      text: qsTr("Metronome visible")
      checked: true
    }

    TcheckBox {
      x: Noo.fontSize()
      id: countChB
      enabled: metroVisibleChB.checked
      text: qsTr("Count up")
      checked: true
    }

//     TcheckBox {
//       id: beforeTickChB
//       text: qsTr("Tick before")
//       checked: SOUND.tickBeforePlay
//     }

    ButtonGroup { buttons: radioRow.children }
    Item {
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width; height: radioRow.height
      MouseArea {
        anchors.fill: parent; hoverEnabled: true
        onEntered: Noo.setStatusTip(qsTr("Detected rhythmic units are rounded (quantization). Shortest units require more rhythmical accuracy."), Item.TopLeft)
        onExited: Noo.setStatusTip("", Item.TopLeft)
      }
      Row {
        id: radioRow
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("round to:"); font.pixelSize: Noo.fontSize(); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
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
    }

    TiconButton {
      text: Noo.TR("QPlatformTheme", "Apply")
      pixmap: Noo.pix("check")
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
        SOUND.setMetronome(tempoSpin.value, beatUnitTumb.currentIndex)
        SOUND.quantization = radio16.checked ? 6 : 12 // See Tsound doc for values explanation
//         SOUND.tickBeforePlay = beforeTickChB.checked
        tempoSpin.value = SOUND.tempo
        accepted()
        close()
      }
    }
  }

  onOpened: { SOUND.stop(); spaceShort.enabled = true; tempoSpin.value = SOUND.tempo; beatUnitTumb.currentIndex = SOUND.beatUnit }
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
