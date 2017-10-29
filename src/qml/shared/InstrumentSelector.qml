/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Tumbler {
  id: instrTumb

  property alias instrument: instrTumb.currentIndex

  width: parent.width
  height: Noo.fontSize() * 10
  visibleItemCount: Math.min(((width / (Noo.fontSize() * 7)) / 2) * 2 - 1, 7)
  model: 8
  delegate: Component {
    Column {
      spacing: Noo.fontSize() / 4
      opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
      scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
      Text {
        font {family: "nootka"; pixelSize: Noo.fontSize() * 3 }
        text: Noo.instr(modelData).glyph
        anchors.horizontalCenter: parent.horizontalCenter
        color: instrTumb.currentIndex === modelData ? activPal.highlightedText : activPal.text
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        width: Noo.fontSize() * 8
        text: Noo.instr(modelData).name.replace(" ", "\n")
        horizontalAlignment: Text.AlignHCenter
        color: activPal.text
        font { bold: instrTumb.currentIndex === modelData; pixelSize: Noo.fontSize() * 0.8 }
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
      startY: Noo.fontSize() * 1.4
      PathLine {
        x: pathView.width
        y: Noo.fontSize() * 1.4
      }
    }
  }
  Rectangle {
    z: -1; width: Noo.fontSize() * 9; height: parent.height * 0.5
    x: parent.width / 2 - width / 2; y: 2
    color: Noo.alpha(activPal.highlight, 100)
    radius: width / 12
  }
}
