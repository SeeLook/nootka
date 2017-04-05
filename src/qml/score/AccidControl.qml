/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtGraphicalEffects 1.0


/**
 * Visibility of this control depends on active (cursor) note of the score
 */
Item {
  id: accidControl

  property bool active: false
  property int selectedId: -1
  property real factor: score.scale * 2
  property string text: ""
  property int alter: accidArray[selectedId + 1]
  readonly property var accidGlyphs: [ "\ue264", "\ue260", "\ue262", "\ue263" ]
  readonly property var accidArray: [ 0, -2, -1, 1, 2 ]

  x: -width - nootkaWindow.fontSize
  y: score.contentY + (score.height - height) / 2
  z: 20
  width: contentColumn.width
  height: contentColumn.height

  Rectangle {
    id: bg
    anchors.fill: parent
    color: activPal.base
    radius: factor / 4
    visible: false
  }

  DropShadow {
    anchors.fill: bg
    horizontalOffset: nootkaWindow.fontSize / 4
    verticalOffset: nootkaWindow.fontSize / 4
    radius: 8.0
    samples: 17
    color: activPal.shadow
    source: bg
  }

  Column {
      id: contentColumn
      spacing: factor / 4
      Repeater {
        model: 4
        Rectangle {
            color: accidControl.selectedId === index ? activPal.highlight : "transparent"
            width: factor * 2
            height: factor * 3
            radius: factor / 5
            visible: score.enableDoubleAccids || index === 1 || index === 2
            Text {
              id: accText
              y: factor * -4.5
              x: (factor * 2 - width) / 2
              height: factor * 3
              font { family: "scorek"; pixelSize: factor * 3 }
              text: accidGlyphs[index]
              color: accidControl.selectedId === index ? activPal.highlightedText : activPal.text
              style: Text.Normal
              styleColor: activPal.highlightedText
            }
            MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onClicked: {
                selectedId = selectedId === index ? -1 : index
                accidControl.text = selectedId > -1 ? accidGlyphs[selectedId] : ""
              }
              onEntered: { accText.style = Text.Outline; hideTimer.stop() }
              onExited: { accText.style = Text.Normal; hideTimer.restart() }
            }
        }
      }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 300 }}

  onActiveChanged: {
    if (active) {
        x = nootkaWindow.fontSize / 2
        hideTimer.stop()
    } else {
        hideTimer.restart()
    }
  }

  Timer {
      id: hideTimer
      interval: 1500
      repeat: false
      onTriggered: x = -width - nootkaWindow.fontSize
  }

}
