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
  property var beatFactor: [ 1, 2, 0.66666666666, 0.5 ]

  Column {
    spacing: Noo.fontSize() / 2

    Row {
      spacing: Noo.fontSize()
      Tumbler {
        id: buTumb
        property int prevIndex: -1
        background: Rectangle { color: activPal.base }
        anchors.verticalCenter: parent.verticalCenter
        height: Noo.fontSize() * 6; width: Noo.fontSize() * 2
        model: tempoBar.beatModel
        visibleItemCount: 3; wrap: true
        currentIndex: SOUND.beatUnit
        onCurrentIndexChanged: {
          if (prevIndex > -1)
            tempoSpin.value = Math.round(tempoSpin.value * (beatFactor[currentIndex] / beatFactor[prevIndex]))
          prevIndex = currentIndex
        }
        delegate: Rectangle {
          color: index === buTumb.currentIndex ? activPal.highlight : activPal.base
          opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
          Text {
            text: modelData
            y: height * 0.325; height: parent.height; width: parent.width
            color: index === buTumb.currentIndex ? activPal.highlightedText : activPal.text
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            font { pixelSize: parent.height * 0.75; family: "Scorek" }
            MouseArea {
              anchors.fill: parent
              onClicked: buTumb.currentIndex = index
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
          TspinBox {
            id: tempoSpin
            focus: false
            from: 40; to: Math.min(240, 180 * beatFactor[buTumb.currentIndex]); editable: true
            value: SOUND.tempo
          }
        }
        Tslider {
          focus: false
          width: parent.width * 0.96
          anchors.horizontalCenter: parent.horizontalCenter
          value: tempoSpin.value
          from: 40; to: tempoSpin.to
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
      focus: true
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
        TradioButton {
          id: radio16
          font { family: "Nootka" }
          text: "G"; textScale: 2.0
          checked: SOUND.quantization === 6
          onClicked: SOUND.quantization = 6
        }
        TradioButton {
          id: radio8
          font { family: "Nootka" }
          text: "F"; textScale: 2.0
          checked: SOUND.quantization === 12
          onClicked: SOUND.quantization = 12
        }
      }
    }

  }

  onAboutToShow: { SOUND.stop(); spaceShort.enabled = true; tempoSpin.value = SOUND.tempo; buTumb.currentIndex = SOUND.beatUnit }
  onAboutToHide: { SOUND.startListen(); spaceShort.enabled = false; SOUND.setMetronome(tempoSpin.value, buTumb.currentIndex) }

  Shortcut { id: spaceShort; sequence: " "; onActivated: tapTempo() }

  property real lastTime: new Date().getTime()

  function tapTempo() {
    var currTime = new Date().getTime()
    if (currTime - lastTime < 1500)
      tempoSpin.value = Math.round(60000 / (currTime - lastTime))
    lastTime = currTime
  }
}
