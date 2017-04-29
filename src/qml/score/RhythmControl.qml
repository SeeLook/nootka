/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7


TipRect {
  id: rhythmControl

  property int selectedId: -1
  property real factor: score.scale * 2
  property bool active: false

  // private
  property var lGlyphs: [ "\ue107", "\ue108", "\ue109", "\ue10a", "\ue10b", "\u0183" ]
  property var rGlyphs: [ "C", "D", "E", "F", "G", "." ]

  x: score.width + nootkaWindow.fontSize
  y: score.contentY + (score.height - height) / 2
  z: 20
  width: contentGrid.width
  height: contentGrid.height

  Grid {
    z: 22
    id: contentGrid
    spacing: factor / 4
    columns: 2

    Repeater {
      model: 12
      ControlButton {
        factor: rhythmControl.factor
        selected: rhythmControl.selectedId === index
        yOffset: factor / 2
        font { family: "nootka"; pixelSize: factor * 2 }
        text: index % 2 === 0 ? lGlyphs[index / 2] : rGlyphs[Math.floor(index / 2)]
        onClicked: {
          selectedId = selectedId === index ? -1 : index
        }
        onEntered: hideTimer.stop()
        onExited: hideTimer.restart()
      }
    }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 300 }}

  onActiveChanged: {
    if (active) {
        x = score.width - nootkaWindow.fontSize / 2 - width
        hideTimer.stop()
    } else {
        hideTimer.restart()
    }
  }

  Timer {
    id: hideTimer
    interval: 1500
    repeat: false
    onTriggered: x = score.width + nootkaWindow.fontSize
  }

}
