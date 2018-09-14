/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


ControlBase {
  id: toolbox

  x: show ? 2 : -width - Noo.fontSize()
  y: score.singleNote || score.meter === Tmeter.NoMeter ? (score.height - height) / 2 : Noo.fontSize() / 2
  visible: !scoreObj.touched

  active: !score.readOnly && (scoreObj.activeNote || (score.noteAdd && score.noteAdd.active))

  factor: Noo.fontSize() * (Noo.isAndroid() ? 1 : 1.2)

  property string rhythmText: Noo.rhythmText(scoreObj.workRhythm)
  property bool triplet: false
  property bool tie: false

  // Accidentals
  property int selectedId: idArray[scoreObj.cursorAlter + 2]
  property string text: selectedId > -1 ? accidGlyphs[selectedId] : ""
  property int alter: accidArray[selectedId + 1]
  readonly property var accidGlyphs: [ "\ue264", "\ue260", "\ue262", "\ue263" ]
  readonly property var accidArray: [ 0, -2, -1, 1, 2 ]
  readonly property var idArray: [ 0, 1, -1, 2, 3 ]

  Component {
    id: ctrlButtonComp
    ControlButton {
      property int rhythm: 1
      property bool rest: false
      factor: toolbox.factor * 0.9
      yOffset: factor * 0.5
      font { family: "nootka"; pixelSize: factor * 2 }
      text: Noo.rhythmText(Noo.rhythm(rhythm, rest, false, false))
      selected: rhythm === scoreObj.workRtmValue && rest === scoreObj.workRtmRest
      onEntered: hideTimer.stop()
      onExited: hideTimer.restart()
    }
  }

  component: Component {
    id: contentComp
    Column {

      Grid {
        columns: score.meter === Tmeter.NoMeter ? 1 : 2
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
          model: 4
          ControlButton {
            factor: toolbox.factor * 0.9
            yOffset: factor * -4.4
            visible: score.enableDoubleAccids || index === 1 || index === 2
            selected: selectedId === index
            font { family: "scorek"; pixelSize: factor * 3 }
            text: accidGlyphs[index]
            onClicked: {
              scoreObj.cursorAlter = accidArray[(selectedId === index ? -1 : index) + 1]
              score.alterText = toolbox.text
            }
            onEntered: hideTimer.stop()
            onExited: hideTimer.restart()
          }
        }
      }

      Rectangle { visible: score.meter !== Tmeter.NoMeter; width: toolbox.width; height: 1; color: activPal.text }

      Grid {
        visible: score.meter !== Tmeter.NoMeter
        columns: 2
        Repeater {
          model: 10
          Loader {
            sourceComponent: ctrlButtonComp
            onLoaded: { item.rhythm = 1 + index / 2; item.rest = index % 2 === 0 }
            Connections {
              target: item
              onClicked: { scoreObj.workRtmValue = item.rhythm; scoreObj.workRtmRest = item.rest }
            }
          }
        }
        Loader { // triplet
          id: tripLoad
          sourceComponent: ctrlButtonComp
          onLoaded: { item.rhythm = 0; item.text = "\u0183" }
          Binding { target: tripLoad.item; property: "selected"; value: toolbox.triplet }
//           Connections {
//             target: tripLoad.item
//             onClicked: { toolbox.triplet = !tripLoad.item.selected; scoreObj.workRhythm = rhythm }
//           }
        }
        Loader { // dot
          id: dotLoad
          sourceComponent: ctrlButtonComp
          onLoaded: { item.rhythm = 0; item.text = "." }
          Binding { target: dotLoad.item; property: "selected"; value: scoreObj.workRtmDot }
          Binding { target: dotLoad.item; property: "enabled"; value: scoreObj.workRtmValue !== Trhythm.Sixteenth }
          Connections {
            target: dotLoad.item
            onClicked: scoreObj.workRtmDot = !scoreObj.workRtmDot
          }
        }
      }

      ControlButton { // tie
        visible: false //score.meter !== Tmeter.NoMeter
        anchors.horizontalCenter: parent.horizontalCenter
        factor: toolbox.factor * 1.2
        selected: toolbox.tie
        height: factor * 1.5
        yOffset: factor * -2.2
        font { family: "nootka"; pointSize: factor * 3.6 }
        text: "\ue18c"
//         onClicked: toolbox.tie = !selected
        onEntered: hideTimer.stop()
        onExited: hideTimer.restart()
      }

      Rectangle { visible: scoreObj.enableTechnical; width: toolbox.width; height: 1; color: activPal.text }

      Grid {
        visible: scoreObj.enableTechnical
        columns: score.meter === Tmeter.NoMeter ? 1 : 2
        Repeater {
          model: [ "\uE610", "\uE612" ]
          ControlButton {
            factor: toolbox.factor * 0.9
            yOffset: factor * -2.5
            font { family: "Scorek"; pixelSize: factor * 2 }
            text: modelData
            onClicked: {
              show = false
              if (scoreObj.selectedItem) {
                var b = index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp
                if (scoreObj.selectedItem.bowing() === b)
                  b = TnoteItem.BowUndefined
                scoreObj.selectedItem.setBowing(b)
              }
            }
            //TODO select bow control according to currently selected note
//             onVisibleChanged: selected = scoreObj.selectedItem && scoreObj.selectedItem.bowing() === (index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp)
            onEntered: hideTimer.stop()
            onExited: hideTimer.restart()
          }
        }
      }
    }
  }

  Connections {
    target: scoreObj
    onScoreWasCleared: show = false
    onEditModeChanged: show = scoreObj.editMode && scoreObj.activeNote
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}

}
