/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  spacing: Noo.fontSize()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width * 0.9

  Tile {
    anchors.horizontalCenter: parent.horizontalCenter
    width: Noo.fontSize() * 12; height: Noo.fontSize() * 12
    bgColor: Qt.tint(nootkaWindow.labelColor, Noo.alpha(activPal.base, 230))
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Text {
      anchors.fill: parent
      horizontalAlignment: Text.AlignHCenter
      y: Noo.fontSize() * -2
      font { pixelSize: Noo.fontSize() * 4; family: "Scorek" }
      text: "\ue043"
      color: activPal.text
    }
  }

  Text {
    width: parent.width
    anchors.horizontalCenter: parent.horizontalCenter
    font { pixelSize: Noo.fontSize() * 1.5; bold: true }
    wrapMode: Text.WordWrap
    text: qsTr("Musical notation for bass guitar uses bass clef (clef F), but the notes sound one octave lower. It means bass guitar is transposing instrument and it transposes one octave down.")
    color: activPal.text
  }
}

