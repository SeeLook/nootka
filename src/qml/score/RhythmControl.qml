/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Score 1.0


ControlBase {
  id: rhythmControl

  signal changed()

  property string rhythmText: "E" // Quarter by default
  property int rtm: Trhythm.Quarter
  property bool rest: false
  property bool dot: false
  property bool triplet: false
  property var rhythm: Noo.rhythm(rtm, rest, dot, triplet)
  property bool tie: false

  property int selectedId: 5 // quarter

  // private
  // unicode: rests & triplet, letters: notes and dot
  readonly property var glyphs: [ "\ue107", "C", "\ue108", "D", "\ue109", "E", "\ue10a", "F", "\ue10b", "G", "\u0183", "."]

  x: show ? score.scoreObj.xLastInActivBar : score.width + nootkaWindow.fontSize + width
  y: score.contentY + (score.height - height) / 2

  component: Component {
      id: contentComp
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
              text: glyphs[index] // index % 2 === 0 ? lGlyphs[index / 2] : rGlyphs[Math.floor(index / 2)]
              onClicked: {
                if (index < 10) {
                    rest = index % 2 === 0
                    rtm = Math.floor(index / 2) + 1
                    selectedId = index
                    dot = false
                    rtmRep.itemAt(11).selected = false
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
                rhythmText = (rtmRep.itemAt(selectedId) ? rtmRep.itemAt(selectedId).text : "E") + (dot ? "." : "")
                rhythmControl.changed()
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
          onClicked: { selected = !selected; tie = selected }
          onEntered: hideTimer.stop()
          onExited: hideTimer.restart()
        }
      }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}

}
