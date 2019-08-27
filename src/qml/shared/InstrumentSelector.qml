/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Tumbler {
  id: instrTumb

  property alias instrument: instrTumb.currentIndex

  width: parent.width
  height: Noo.fontSize() * 10
  visibleItemCount: Math.min(((width / (height * 0.7)) / 2) * 2 - 1, 7)
  model: 8
  delegate: Component {
    Item {
      opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
      scale: (1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)) * (textMa.pressed ? 0.8 : 1.0)
      Text {
        id: instrGlyph
        property color tc: Noo.randomColor()
        font {family: "nootka"; pixelSize: instrTumb.height * 0.3 }
        text: Noo.instr(modelData).glyph
        anchors.horizontalCenter: parent.horizontalCenter
        color: instrTumb.currentIndex === modelData ? activPal.highlightedText : tc
        MouseArea {
          id: textMa
          anchors.fill: parent
          onClicked: instrTumb.currentIndex = modelData
        }
      }
      Text {
        anchors { horizontalCenter: parent.horizontalCenter; top: instrGlyph.bottom }
        width: instrTumb.height * 0.8
        text: Noo.instr(modelData).name.replace(" ", "\n")
        horizontalAlignment: Text.AlignHCenter
        color: activPal.text
        font { bold: instrTumb.currentIndex === modelData; pixelSize: instrTumb.height * 0.08 }
      }
    }
  }
  contentItem: PathView {
    id: pathView
    model: instrTumb.model
    delegate: instrTumb.delegate
    clip: true
    pathItemCount: instrTumb.visibleItemCount + 1
    preferredHighlightBegin: 0.5
    preferredHighlightEnd: 0.5
    dragMargin: width / 2
    path: Path {
      startX: 0
      startY: instrTumb.height * 0.14
      PathLine {
        x: pathView.width
        y: instrTumb.height * 0.14
      }
    }
  }
  Rectangle {
    z: -1; width: instrTumb.height * 0.9; height: parent.height * 0.5
    x: parent.width / 2 - width / 2; y: 2
    color: activPal.highlight
    radius: width / 12
  }
}
