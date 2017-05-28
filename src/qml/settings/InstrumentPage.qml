/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0
import Nootka 1.0
import "../"


Flickable {
  clip: true
  width: parent.width
  height: parent.height
  contentHeight: instrCol.height + nootkaWindow.fontSize * 2

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

  Column {
    id: instrCol
    width: parent.width
    spacing: nootkaWindow.fontSize / 2
    Text {
      text: qsTr("Instrument")
      anchors.horizontalCenter: parent.horizontalCenter
    }
    InstrumentSelector {
      anchors.horizontalCenter: parent.horizontalCenter
      instrument: GLOB.instrument.type
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
          id: score
          height: nootkaWindow.fontSize * 20
          width: Math.min(parent.width * 0.9, nootkaWindow.fontSize * 26)
          anchors.horizontalCenter: parent.horizontalCenter
          meter: Tmeter.NoMeter
          Component.onCompleted: {
            for (var s = 1; s <= GLOB.tuning.stringNumber; ++s)
              score.addNote(GLOB.tuning.string(s))
          }
        }
      }
      description: qsTr("Select appropriate tuning from the list or prepare your own.") + "<br>" + 
                    qsTr("Remember to select the appropriate clef in Score settings.")
    }

    Tile {
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("preferred accidentals:"); anchors.verticalCenter: parent.verticalCenter }
        RadioButton { id: prefSharpRadio; text: qsTr("# - sharps"); checked: !GLOB.preferFlats }
        RadioButton { id: prefFlatRadio; text: qsTr("b - flats"); checked: GLOB.preferFlats }
      }
      description: qsTr("Choose which accidentals will be shown on the staff.")
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

    Tile {
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a pointer on an instrument"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: fingerColorButt; color: GLOB.fingerColor }
      }
    }
    Tile {
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a selection"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: selectedColorButt; color: GLOB.selectedColor }
      }
    }

  }

  function save() {
    GLOB.fingerColor = fingerColorButt.color
    GLOB.selectedColor = selectedColorButt.color
    GLOB.preferFlats = prefFlatRadio.checked
  }

  function defaults() {
    fingerColorButt.color = Qt.rgba(1, 0, 0.5, 0.78)
    selectedColorButt.color = Qt.rgba(0.2, 0.6, 1.0, 1.0)
    prefSharpRadio.checked = true
  }
}
