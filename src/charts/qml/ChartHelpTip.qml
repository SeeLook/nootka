/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TipRect {
  id: helpTip
  property alias text: txt.text

  shadowRadius: Noo.fontSize()
  width: txt.width  + 2 * Noo.fontSize()
  height: txt.height + 2 * Noo.fontSize()
  z: 150

  border { color: GLOB.correctColor; width: 1 }
  color: Qt.tint(activPal.base, Noo.alpha(GLOB.correctColor, 50))

  anchors { centerIn: parent }

  Column {
    width: parent.width
    padding: Noo.fontSize()
    Text {
      z: 200
      id: txt
      color: activPal.text
      wrapMode: Text.WordWrap; textFormat: Text.RichText; horizontalAlignment: Text.AlignHCenter
      onLinkActivated: analyzeWindow.openExam()
      MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
      }
    }
  }
}
