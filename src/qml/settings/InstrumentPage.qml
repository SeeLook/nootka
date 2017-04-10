/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0
import Nootka 1.0


Flickable {
  clip: true
  width: parent.width
  height: parent.height
  contentHeight: instrCol.height + nootkaWindow.fontSize * 2

  property var instrGlyphs: ["v", "h", "j", "i"]

  Column {
    id: instrCol
    width: parent.width
    spacing: nootkaWindow.fontSize / 2
    Text {
      text: qsTr("Instrument")
      anchors.horizontalCenter: parent.horizontalCenter
    }
    Row {
      spacing: nootkaWindow.fontSize
      anchors.horizontalCenter: parent.horizontalCenter
      Repeater {
        model: 4
        Button {
          contentItem: Text {
            font {family: "nootka"; pixelSize: 50 }
            text: instrGlyphs[index]
            anchors.horizontalCenter: parent.horizontalCenter
          }
        }
      }
    }

    Grid {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: nootkaWindow.fontSize
      columns: parent.width < nootkaWindow.fontSize * 50 ? 1 : 2
      Row {
        spacing: nootkaWindow.fontSize
        Text { text: qsTr("number of frets:"); anchors.verticalCenter: parent.verticalCenter }
        SpinBox {}
      }
      Row {
        spacing: nootkaWindow.fontSize
        Text { text: qsTr("number of strings:"); anchors.verticalCenter: parent.verticalCenter }
        SpinBox {}
      }
    }

    Tile {
      Column {
        spacing: nootkaWindow.fontSize / 4
        width: parent.width
        Row {
          spacing: nootkaWindow.fontSize
          anchors.horizontalCenter: parent.horizontalCenter
          Text { text: qsTr("tuning of the guitar"); anchors.verticalCenter: parent.verticalCenter }
          ComboBox {
            model: GLOB.instrument.type === Tinstrument.BassGuitar ? Noo.bassTunings() : Noo.guitarTunings()
            delegate: ItemDelegate {
              text: modelData
            }
          }
        }
        Score {
          width: parent.width * 0.9
          height: nootkaWindow.fontSize * 20
          anchors.horizontalCenter: parent.horizontalCenter
        }
      }
      description: qsTr("Select appropriate tuning from the list or prepare your own.") + "<br>" + 
                    qsTr("Remember to select the appropriate clef in Score settings.")
    }

    Tile {
      CheckBox {
        text: qsTr("show all possibilities of a note")
        anchors.horizontalCenter: parent.horizontalCenter
      }
      description: qsTr("As you know, the same note can be played in several places on the fingerboard.<br>If checked, all of them will be shown.")
    }

    Tile {
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("marked frets", "or frets with dots/marks"); anchors.verticalCenter: parent.verticalCenter }
        TextField {
          maximumLength: 25
          validator: RegExpValidator { regExp: /([1-2]{0,1}[0-9]{1,2}!{0,1},){0,7}/ }
        }
      }
      description: qsTr("Put numbers of frets marked with dot. Separate the numbers with comma. Add ! (exclamation mark) after a number to paint a dot twice.")
    }
  }
}
