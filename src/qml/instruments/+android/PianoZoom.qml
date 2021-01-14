/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020=2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TipRect {
  id: octaveView

  property bool zoomed: false
  property int octave: (octaveView.x / (instrItem.width - width * 0.99)) * octCount
  property int octCount: instrItem.keysNumber / 7
  property int zoomKey: instrItem.selectedKey && Math.floor(instrItem.selectedKey.nr / 7) === octave ? instrItem.selectedKey.nr % 7 : -1

  enabled: instrItem.enabled
  scale: zoomed ? 1 : 0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation {} }
  Behavior on x { enabled: GLOB.useAnimations; NumberAnimation {} }

  width: keyRow.width; height: keyRow.height
  y: parent.height - height - 1
  z: 99
  color: "black"

  onEnabledChanged: {
    if (!instrItem.enabled)
      zoomed = false // reset zoom/scale when piano becomes disable
  }

  Item { // overlay
    enabled: instrItem.enabled
    parent: octaveView.parent
    width: parent.width; height: zoomed ? parent.height : instrItem.height
    y: nootkaWindow.height - height
    z: 98
    MouseArea {
      anchors.fill: parent
      hoverEnabled: !instrItem.readOnly
      onClicked: {
        if (!zoomed)
          octaveView.x = instrItem.zoomViewX(mouse.x, octaveView.height / 4)
        octaveView.zoomed = !octaveView.zoomed
      }
    }
  }

  MouseArea {
    enabled: zoomed
    anchors.fill: parent
    drag.target: octaveView
    drag.minimumX: 0; drag.maximumX: instrItem.width - width
    drag.axis: Drag.XAxis
    drag.threshold: height * 0.3
    cursorShape: drag.active ? Qt.DragMoveCursor : Qt.ArrowCursor
    drag.filterChildren: true

    Row {
      id: keyRow
      Repeater {
        id: keyRep
        model: 7
        PianoKeyWhite {
          nr: index
          enabled: zoomed
          height: Math.max(Noo.fingerPixels() * 4, instrItem.height * 1.1)
          width: height / 4
          onClicked: {
            var k = instrItem.getKey(octave * 7 + key.nr)
            instrItem.selectedKey = key.z > 0 ? k.black : k
          }
        }
      }
    }

    Rectangle { // piano key highlight
      id: keyHigh
      parent: zoomKey > -1 ? (instrItem.selectedKey.z > 0 ? keyRep.itemAt(zoomKey).black : keyRep.itemAt(zoomKey)) : null
      anchors.fill: parent ? parent : undefined
      color: GLOB.selectedColor
      border { width: Math.round(width / 16); color: "black" }
      radius: width / 5
      z: 2
    }

    Rectangle {
      width: keyRow.width; height: Noo.fontSize() * (Noo.isAndroid() ? 1 : 1.5) + 2
      y: -2
      color: octave % 2 ? "#303030" : "black"
      Text {
        anchors.centerIn: parent
        text: octaveName(firstOctave + octave) + (GLOB.scientificOctaves ? "  [%1]".arg(firstOctave + index + 3) : "")
        font { pixelSize: parent.height * 0.8 }
        color: "white"
      }
    }
  }
}
