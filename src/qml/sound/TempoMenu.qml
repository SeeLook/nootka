/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  x: Noo.isAndroid() ? (nootkaWindow.width - width) / 2 : 0
  y: Noo.isAndroid() ? (nootkaWindow.height - height) / 2 : 0

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

        background: TipRect { color: buTumb.enabled ? activPal.base : disdPal.base; radius: 0 }
        anchors.verticalCenter: parent.verticalCenter
        height: Noo.fontSize() * 6; width: Noo.fontSize() * 2
        model: [ "\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3" ]
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

        MouseArea {
          anchors.fill: parent
          z: -1
          onWheel: {
            var ci = buTumb.currentIndex
            if (wheel.angleDelta.y > 0)
              ci--
            else if (wheel.angleDelta.y < 0)
              ci++
            if (ci >= buTumb.count)
              ci = 0
            else if (ci < 0)
              ci = buTumb.count - 1
            buTumb.currentIndex = ci
          }
        }
      } // Tumbler

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

    } // Row

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
      visible: !Noo.isAndroid()
      text: qsTr("Metronome visible")
      checked: true
    }

    TcheckBox {
      id: countChB
      visible: !Noo.isAndroid()
      x: Noo.fontSize()
      enabled: metroVisibleChB.checked
      text: qsTr("Count up")
      checked: true
    }

    Rectangle {
      visible: Noo.isAndroid()
      width: parent.width
      height: Noo.fontSize() * 2.8
      anchors.horizontalCenter: parent.horizontalCenter
      color: ma.containsPress ? Noo.alpha(activPal.highlight, 50) : "transparent"
      Rectangle { color: activPal.text; width: parent.width; height: 1; y: -1 }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: "\u0190"; font { family: "Nootka"; pixelSize: Noo.fontSize() * 2.5 }
        color: SOUND.tickBeforePlay ? "#00a0a0" : activPal.text
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        x: Noo.fontSize() * 4; font.pixelSize: Noo.fontSize()
        text: qsTr("Count up")
      }
      MouseArea {
        id: ma
        anchors.fill: parent
        onClicked: SOUND.tickBeforePlay = !SOUND.tickBeforePlay
        onPressAndHold: Noo.setStatusTip(Noo.TR("TempoBar", "Countdown before playing or listening."))
        onReleased: Noo.setStatusTip("")
      }
      Rectangle { color: activPal.text; width: parent.width; height: 1; y: parent.height + Noo.fontSize() / 4 - 1 }
    }
    Rectangle {
      visible: Noo.isAndroid()
      width: parent.width
      height: Noo.fontSize() * 2.8
      anchors.horizontalCenter: parent.horizontalCenter
      color: ma2.containsPress ? Noo.alpha(activPal.highlight, 50) : "transparent"
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: "\u018f"; font { family: "Nootka"; pixelSize: Noo.fontSize() * 2.5 }
        color: SOUND.tickDuringPlay ? "#00a0a0" : activPal.text
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        x: Noo.fontSize() * 4; font.pixelSize: Noo.fontSize()
        text: Noo.TR("TempoBar", "Audible metronome")
      }
      MouseArea {
        id: ma2
        anchors.fill: parent
        onClicked: SOUND.tickDuringPlay = !SOUND.tickDuringPlay
        onPressAndHold: Noo.setStatusTip(Noo.TR("TempoBar", "Audible metronome") + ".<br>" + Noo.TR("TempoBar", "Use earphones! Otherwise ticking will disturb proper pitch detection!"))
        onReleased: Noo.setStatusTip("")
      }
      Rectangle { color: activPal.text; width: parent.width; height: 1; anchors.bottom: parent.bottom }
    }

    Item {
      ButtonGroup { buttons: radioRow.children }
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width; height: radioRow.height
      enabled: !executor
      MouseArea {
        anchors.fill: parent; hoverEnabled: !Noo.isAndroid()
        onEntered: Noo.setStatusTip(qsTr("Detected rhythmic units are rounded (quantization). Shortest units require more rhythmical accuracy."), Item.TopLeft)
        onExited: Noo.setStatusTip("", Item.TopLeft)
        onPressAndHold: {
          if (Noo.isAndroid())
            Noo.setStatusTip(qsTr("Detected rhythmic units are rounded (quantization). Shortest units require more rhythmical accuracy."))
        }
        onReleased: {
          if (Noo.isAndroid())
            Noo.setStatusTip("")
        }
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
