/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../settings"
import "../"


Flickable {
  property alias is7B: seventhNote.is7B
  property alias doubleAccids: dblAccidsChB.checked
  property alias enharmNames: enharmChB.checked
  property alias useKeys: keySignChB.checked

  clip: true
  contentHeight: optCol.height
  contentWidth: width

  Column {
    id: optCol
    width: parent.width
    spacing: nootkaWindow.fontSize
    Text {
      width: parent.width
      font { pixelSize: nootkaWindow.fontSize * 2; bold: true }
      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      color: activPal.text
      text: qsTr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?")
    }
    Select7note {
      id: seventhNote
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Tile {
      width: innerCol.width + nootkaWindow.fontSize * 2
      Column {
        id: innerCol
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        TcheckBox {
          id: dblAccidsChB
          text: qsTr("I know about double sharps (x) and double flats (bb)")
        }
        TcheckBox {
          id: enharmChB
          text: qsTr("I know that e# is the same as f")
        }
        TcheckBox {
          id: keySignChB
          text: qsTr("I know about key signatures")
        }
      }
    }
  }
}
