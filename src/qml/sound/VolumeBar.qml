/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0


Item {
  id: volBar

  property real volume: 0.05
  property real minVol: 0.4

  onWidthChanged: tc.resize(volBar.width - minText.width - noteText.width * 2)

  TtickColors { id: tc }

  Rectangle {
      id: bgRect
      anchors.fill: parent
      color: activPal.window
  }

  Text {
      id: minText
      anchors.top: parent.Top
      anchors.left: parent.Left
      anchors.verticalCenter: parent.verticalCenter
      text: " " + Math.round(minVol * 100) + "% "
      color: activPal.text
  }

  Repeater {
      id: vRep
      model: (volBar.width - minText.width - noteText.width * 2) / (pitchView.tickGap + pitchView.tickWidth)
      Rectangle {
        color: {
          var tickNr = vRep.model * index / 100
          tickNr <= volBar.volume * 100 ? tc.colorAt(tickNr) : disdPal.text
        }
        width: (index * 9) < minVol * (volBar.width - minText.width - noteText.width * 1.5) ? pitchView.tickWidth / 2 : pitchView.tickWidth
        radius: pitchView.tickWidth / 2
        height: pitchView.tickWidth * 1.5 + ((volBar.height - pitchView.tickWidth * 4) / vRep.model) * index
        y: (parent.height - height) / 2
        x: minText.width + (index * pitchView.tickGap) + (index + 1) * pitchView.tickWidth
      }
  }

  Text {
      id: noteText
      anchors.top: parent.Top
      x: volBar.width - width * 1.5
      anchors.verticalCenter: parent.verticalCenter
      font.family: "Nootka"
      font.pixelSize: volBar.height
      text: "r"
      color: activPal.text
  }

  DropShadow {
      id: knobShad
      anchors.fill: knob
      horizontalOffset: pitchView.tickWidth
      verticalOffset: pitchView.tickWidth
      color: activPal.shadow
      radius: 8.0
      source: knob
      visible: false
  }

  Rectangle {
      id: knob
      x: minVol * volBar.width
      y: (volBar.height - height) / 2
      visible: false
      height: volBar.height * 0.9
      width: height
      radius: height / 2
      color: activPal.highlight
      Rectangle {
        height: parent.height / 2.5
        width: height
        anchors.centerIn: parent
        radius: height / 2
        color: activPal.highlightedText
      }
  }

  MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      acceptedButtons: Qt.LeftButton

      onEntered: {
        knobShad.visible = true
      }
      onPositionChanged: {
        if (pressedButtons && mouseX > vRep.itemAt(1).x && mouseX < vRep.itemAt(vRep.model - 2).x) {
          var mv = mouseX / volBar.width
          if (mv > 0.1 && mv < 0.9) {
            minVol = (mouseX - knob.width) / volBar.width
            knob.x = mouseX - knob.width / 2
          }
        }
      }
      onExited: {
        knobShad.visible = false
      }
    }
}
