/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Nootka 1.0
import "../"
import "../settings"


Tflickable {
  id: wizardOpts

  height: parent.height
  contentWidth: width; contentHeight: optCol.height + NOO.factor() * 2

  function setOptions() {
    GLOB.seventhIsB = seventhNote.is7B
    GLOB.enableDoubleAccids = dblAccidsChB.checked
    GLOB.keySignatureEnabled = keySignChB.checked
    GLOB.showEnharmNotes = enharmChB.checked
    GLOB.noteNameStyle = seventhNote.style
  }

  Column {
    id: optCol
    width: parent.width
    spacing: NOO.factor() * (NOO.isAndroid() ? 1 : 2)
    y: Math.max(0, (wizardOpts.height - height) / 2)

    Text {
      width: parent.width
      font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.2 : 1.5); bold: true }
      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      color: activPal.text
      text: qsTr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?")
    }

    Select7note {
      id: seventhNote
      anchors.horizontalCenter: parent.horizontalCenter
      style: lettersRadio.checked ? (is7B ? Nootka.English_Bb : Nootka.Norsk_Hb) : (Qt.locale().name.indexOf("ru") === -1 ? Nootka.Italiano_Si : Nootka.Russian_Ci)
      is7B: NOO.note7translated().toLowerCase() === "b"
    }

    Row {
      spacing: NOO.factor()
      anchors.horizontalCenter: parent.horizontalCenter
      TradioButton {
        id: lettersRadio
        text: qsTranslate("NameStyleSelector", "letter names")
        checked: NOO.keyNameTranslated() !== "solfege"
      }
      TradioButton {
        text: qsTranslate("NameStyleSelector", "solfege names")
        checked: NOO.keyNameTranslated() === "solfege"
      }
    }

    Tile {
      bgColor: Qt.tint(nootkaWindow.labelColor, NOO.alpha(activPal.base, 230))
      bgBorder { width: 2; color: nootkaWindow.labelColor }
      width: innerCol.width + NOO.factor() * 4
      Column {
        id: innerCol
        spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)
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
