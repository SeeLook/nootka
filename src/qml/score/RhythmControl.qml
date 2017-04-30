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
  readonly property var lGlyphs: [ "\ue107", "\ue108", "\ue109", "\ue10a", "\ue10b", "\u0183" ] // rests & triplet
  readonly property var rGlyphs: [ "C", "D", "E", "F", "G", "." ] // individual notes & dot
  property bool show: false

  x: show ? score.scoreObj.xLastInActivBar : score.width + nootkaWindow.fontSize + width

  y: score.contentY + (score.height - height) / 2
  z: 20
  width: contentCol.width
  height: contentCol.height

  Column {
    id: contentCol

    Grid {
      columns: 2

      Repeater {
        model: 12
        ControlButton {
          factor: rhythmControl.factor * 0.9
          selected: rhythmControl.selectedId === index
          yOffset: factor / 2
          font { family: "nootka"; pixelSize: factor * 1.8 }
          text: index % 2 === 0 ? lGlyphs[index / 2] : rGlyphs[Math.floor(index / 2)]
          onClicked: {
            selectedId = selectedId === index ? -1 : index
          }
          onEntered: hideTimer.stop()
          onExited: hideTimer.restart()
        }
      }
    }

    ControlButton { // tie
      anchors.horizontalCenter: parent.horizontalCenter
      factor: rhythmControl.factor * 1.2
      selected: rhythmControl.selectedId === 12
      height: factor * 1.5
      yOffset: -factor * 1.5
      font { family: "nootka"; pixelSize: factor * 3.6 }
      text: "\ue18c"
      onClicked: {
        selectedId = 12
      }
      onEntered: hideTimer.stop()
      onExited: hideTimer.restart()
    }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}

  onActiveChanged: {
    if (active) {
        show = true
        hideTimer.stop()
    } else {
        hideTimer.restart()
    }
  }

  Timer {
    id: hideTimer
    interval: 1500
    repeat: false
    onTriggered: show = false
  }

}
