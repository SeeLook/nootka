/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0


Item {
  id: volBar

  property real volume: 0.05

  TtickColors { id: tc; width: volBar.width - minVolText.width - noteText.width * 2; divisor: pitchView.tickGap + pitchView.tickWidth }

  MouseArea {
    id: area
//     enabled: pitchView.active
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton
    onPositionChanged: {
      if (pressedButtons && mouseX > vRep.itemAt(1).x && mouseX < vRep.itemAt(vRep.model - 2).x) {
        var mv = (mouseX - minVolText.width) / tc.width
        if (mv > 0.1 && mv < 0.9)
          GLOB.minVolume = mv
      }
    }
    onEntered: Noo.setStatusTip(qsTr("Volume level of input sound.") + "<br>" + qsTr("Drag a knob to adjust minimum input volume."), Item.TopLeft)
    onExited: Noo.setStatusTip("", Item.TopLeft)
  }

  Text {
      id: minVolText
      anchors { top: parent.Top; left: parent.Left; verticalCenter: parent.verticalCenter }
      text: " " + Math.round(GLOB.minVolume * 100) + "% "
      color: pitchView.active ? activPal.text : disdPal.text
      font.pixelSize: parent.height / 2
      width: parent.height * 1.2
  }

  Repeater {
      id: vRep
      model: tc.width / tc.divisor
      Rectangle {
        color: pitchView.active ? (index < volume * vRep.model ? tc.colorAt(index) : activPal.text) : disdPal.text
        width: index <= GLOB.minVolume * vRep.model ? pitchView.tickWidth / 2 : pitchView.tickWidth
        radius: pitchView.tickWidth / 2
        height: pitchView.tickWidth * 1.5 + ((volBar.height - pitchView.tickWidth * 4) / vRep.model) * index
        y: (parent.height - height) / 2
        x: minVolText.width + (index * pitchView.tickGap) + (index + 1) * pitchView.tickWidth
      }
  }

  Rectangle {
    x: volBar.width - width
    width: noteText.width * 1.5
    height: parent.height
    color: ma.containsMouse ? activPal.highlight : "transparent"
    radius: height / 5
    Text {
        id: noteText
        anchors.centerIn: parent
        font.family: "Nootka"
        font.pixelSize: volBar.height
        text: "r"
        color: pitchView.active ? "red" : activPal.text
    }
    MouseArea {
      id: ma
      anchors.fill: parent
      onClicked: pitchView.paused()
      hoverEnabled: true
      onEntered: Noo.setStatusTip(qsTr("Start/stop pitch detection") + "<br><b>(Ctrl+M)</b>", Item.TopLeft)
      onExited: Noo.setStatusTip("", Item.TopLeft)
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
      visible: pitchView.active && (area.pressed || area.containsMouse)
  }

  Rectangle {
      id: knob
      x: minVolText.width + GLOB.minVolume * tc.width - radius
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

  Shortcut { sequence: "M"; onActivated: pitchView.paused() }
}
