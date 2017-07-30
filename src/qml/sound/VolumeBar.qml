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


  TtickColors { id: tc; width: volBar.width - minText.width - noteText.width * 2; divisor: pitchView.tickGap + pitchView.tickWidth }

  MouseArea {
    id: area
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton
    onPositionChanged: {
      if (pressedButtons && mouseX > vRep.itemAt(1).x && mouseX < vRep.itemAt(vRep.model - 2).x) {
        var mv = (mouseX - minText.width) / tc.width
        if (mv > 0.1 && mv < 0.9)
          minVol = mv
      }
    }
  }

  Text {
      id: minText
      anchors { top: parent.Top; left: parent.Left; verticalCenter: parent.verticalCenter }
      text: " " + Math.round(minVol * 100) + "% "
      color: activPal.text
      font.pixelSize: parent.height / 2
      width: parent.height * 1.2
  }

  Repeater {
      id: vRep
      model: tc.width / tc.divisor
      Rectangle {
        color: index < volume * vRep.model ? tc.colorAt(index) : activPal.text
        width: index <= minVol * vRep.model ? pitchView.tickWidth / 2 : pitchView.tickWidth
        radius: pitchView.tickWidth / 2
        height: pitchView.tickWidth * 1.5 + ((volBar.height - pitchView.tickWidth * 4) / vRep.model) * index
        y: (parent.height - height) / 2
        x: minText.width + (index * pitchView.tickGap) + (index + 1) * pitchView.tickWidth
      }
  }

  Text {
      id: noteText
      x: volBar.width - width * 1.5
      anchors.verticalCenter: parent.verticalCenter
      font.family: "Nootka"
      font.pixelSize: volBar.height
      text: "r"
      color: activPal.text
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: noteText.color = activPal.highlight
        onExited: noteText.color = activPal.text
      }
  }

  DropShadow {
      id: knobShad
      anchors.fill: knob
      horizontalOffset: pitchView.tickWidth
      verticalOffset: pitchView.tickWidth
      color: activPal.shadow
      radius: 8.0
      source: knob
      visible: area.pressed || area.containsMouse
  }

  Rectangle {
      id: knob
      x: minText.width + minVol * tc.width - radius
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

}
