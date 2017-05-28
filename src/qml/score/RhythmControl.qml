/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Score 1.0


TipRect {
  id: rhythmControl

  signal changed()

  property string rhythmText: (rtmRep.itemAt(selectedId) ? rtmRep.itemAt(selectedId).text : "E") + (dot ? "." : "")
  property int rtm: Trhythm.Quarter
  property bool rest: false
  property bool dot: false
  property bool triplet: false
  property var rhythm: Noo.rhythm(rtm, rest, dot, triplet)
  property alias tie: tieButt.selected
  property bool active: false
  property real factor: score.scale * 2

  property int selectedId: 5 // quarter

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
        id: rtmRep
        model: 12
        ControlButton {
          factor: rhythmControl.factor * 0.9
          selected: index < 10 ? rhythmControl.selectedId === index : false
          yOffset: factor / 2
          font { family: "nootka"; pixelSize: factor * 1.8 }
          text: index % 2 === 0 ? lGlyphs[index / 2] : rGlyphs[Math.floor(index / 2)]
          onClicked: {
            if (index < 10) {
                rest = index % 2 === 0
                rtm = Math.floor(index / 2) + 1
                selectedId = index
            } else {
                selected = !selected
                if (index === 10) {
                    triplet = selected
                    if (selected) {
                      dot = false
                      rtmRep.itemAt(11).selected = false
                    }
                } else {
                    dot = selected && selectedId < 8 // no dot for 16th-s
                    if (dot) {
                      triplet = false
                      rtmRep.itemAt(10).selected = false
                    }
                }
            }
            rhythmControl.changed()
          }
          onEntered: hideTimer.stop()
          onExited: hideTimer.restart()
        }
      }
    }

    ControlButton { // tie
      id: tieButt
      anchors.horizontalCenter: parent.horizontalCenter
      factor: rhythmControl.factor * 1.2
      selected: rhythmControl.selectedId === 12
      height: factor * 1.5
      yOffset: -factor * 1.5
      font { family: "nootka"; pixelSize: factor * 3.6 }
      text: "\ue18c"
      onClicked: selected = !selected
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
    interval: 1000
    repeat: false
    onTriggered: show = false
  }

}
