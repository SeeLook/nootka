/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

import Nootka 1.0
import "../"


Item {
  id: volBar

  property real volume: 0.05
  property bool active: true
  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.4

  TtickColors { id: tc; width: volBar.width - minVolText.width; divisor: tickGap + tickWidth }

  MouseArea {
    id: area
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: active ? Qt.LeftButton : Qt.NoButton
    onPositionChanged: {
      if (pressedButtons && mouseX > vRep.itemAt(1).x && mouseX < vRep.itemAt(vRep.model - 2).x) {
        var mv = (mouseX - minVolText.width) / tc.width
        if (mv > 0.1 && mv < 0.9)
          GLOB.minVolume = mv
      }
    }
    onEntered: Noo.setStatusTip(qsTr("Volume level of input sound.") + (active ? "<br>" + qsTr("Drag a knob to adjust minimum input volume.") : ""),
                                Item.TopLeft)
    onExited: Noo.setStatusTip("", Item.TopLeft)
  }

  Text {
    id: minVolText
    anchors { top: parent.Top; left: parent.Left; verticalCenter: parent.verticalCenter }
    text: " " + Math.round(GLOB.minVolume * 100) + "% "
    color: active ? activPal.text : disdPal.text
    font.pixelSize: parent.height / 2
    width: parent.height * 1.2
  }

  Repeater {
    id: vRep
    model: Math.max(tc.width / tc.divisor, 0)
    Rectangle {
      color: active ? (index < volume * vRep.model ? tc.colorAt(index) : activPal.text) : disdPal.text
      width: index <= GLOB.minVolume * vRep.model ? tickWidth / 2 : tickWidth
      radius: tickWidth / 2
      height: tickWidth * 1.5 + ((volBar.height - tickWidth * 4) / vRep.model) * index
      y: (parent.height - height) / 2
      x: minVolText.width + (index * tickGap) + (index + 1) * tickWidth
    }
  }

  TipRect {
    scale: Noo.isAndroid() && area.pressed ? 1.3 : 1
    x: minVolText.width + GLOB.minVolume * tc.width - radius
    y: (volBar.height - height) / 2
    horizontalOffset: area.pressed ? 0 : tickWidth
    verticalOffset: horizontalOffset
    visible: active && (Noo.isAndroid() || area.pressed || area.containsMouse)
    height: volBar.height * 0.9; width: height; radius: height / 2
    color: activPal.highlight
    Rectangle {
      anchors.fill: parent; scale: 0.5
      radius: height / 2
      color: activPal.highlightedText
    }
  }
}
