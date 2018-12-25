/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0
import "../settings"
import "../"


Column {
  property alias is7B: seventhNote.is7B
  property alias style: seventhNote.style
  property alias doubleAccids: dblAccidsChB.checked
  property alias enharmNames: enharmChB.checked
  property alias useKeys: keySignChB.checked

  width: parent.width
  spacing: Noo.fontSize() * 2
  y: Math.max(0, (wizardOpts.height - height) / 2)

  Text {
    width: parent.width
    font { pixelSize: Noo.fontSize() * 2; bold: true }
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?")
  }

  Select7note {
    id: seventhNote
    anchors.horizontalCenter: parent.horizontalCenter
    style: lettersRadio.checked ? (is7B ? Nootka.English_Bb : Nootka.Norsk_Hb) : (Qt.locale().name.indexOf("ru") === -1 ? Nootka.Italiano_Si : Nootka.Russian_Ci)
    is7B: Noo.note7translated().toLowerCase() === "b"
  }

  Row {
    spacing: Noo.fontSize()
    anchors.horizontalCenter: parent.horizontalCenter
    RadioButton {
      id: lettersRadio
      text: qsTranslate("TnotationRadioGroup", "letter names")
      checked: Noo.keyNameTranslated() !== "solfege"
    }
    RadioButton {
      text: qsTranslate("TnotationRadioGroup", "solfege names")
      checked: Noo.keyNameTranslated() === "solfege"
    }
  }

  Tile {
    bgColor: Qt.tint(nootkaWindow.labelColor, Noo.alpha(activPal.base, 230))
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    width: innerCol.width + Noo.fontSize() * 2
    Column {
      id: innerCol
      spacing: Noo.fontSize()
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
