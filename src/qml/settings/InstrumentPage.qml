/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import Nootka 1.0
import "../"


Flickable {
  clip: true
  width: parent.width
  height: parent.height
  contentHeight: instrCol.height + nootkaWindow.fontSize * 2
  contentWidth: Math.max(width, nootkaWindow.fontSize * 35)

  property bool first: true // read props first time from GLOB but when instrument changed then from its profile

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

  Column {
    id: instrCol
    width: parent.width
    spacing: nootkaWindow.fontSize / 2
    Text {
      text: qsTr("Instrument")
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
    }
    InstrumentSelector {
      id: instrSel
      anchors.horizontalCenter: parent.horizontalCenter
      instrument: GLOB.instrument.type
      onInstrumentChanged: { // load instrument profile
        if (first) {
            transp.shift = GLOB.transposition
            prefFlatRadio.checked = GLOB.preferFlats
            first = false
        } else {
            var ins = Noo.instr(instrument)
            transp.shift = ins.transposition
            prefFlatRadio.checked = ins.isSax ? true : false
            prefSharpRadio.checked = ins.isSax ? false : true
        }
      }
    }

    Grid {
      visible: Noo.instr(instrSel.instrument).isGuitar
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: nootkaWindow.fontSize
      columns: parent.width < nootkaWindow.fontSize * 50 ? 1 : 2
      horizontalItemAlignment: Grid.AlignHCenter
      Row {
        spacing: nootkaWindow.fontSize
        Text { text: qsTr("number of frets:"); anchors.verticalCenter: parent.verticalCenter }
        SpinBox { id: fretsNrSpin; from: 15; to: 24; value: GLOB.fretNumber }
      }
      Row {
        enabled: false // TODO: not implemented yet
        spacing: nootkaWindow.fontSize
        Text { text: qsTr("number of strings:"); anchors.verticalCenter: parent.verticalCenter }
        SpinBox { id: stringNrSpin; from: 3; to: 6; value: GLOB.stringNumber() }
      }
    }

    Tile {
      visible: instrSel.instrument <= Tinstrument.BassGuitar
      Column {
        spacing: nootkaWindow.fontSize / 4
        width: parent.width
        Row {
          spacing: nootkaWindow.fontSize
          anchors.horizontalCenter: parent.horizontalCenter
          Text { text: qsTr("tuning of the guitar"); anchors.verticalCenter: parent.verticalCenter }
          ComboBox {
            width: nootkaWindow.fontSize * 20
            model: GLOB.instrument.type === Tinstrument.BassGuitar ? Noo.bassTunings() : Noo.guitarTunings()
            delegate: ItemDelegate { text: modelData }
          }
        }
        Score {
          id: score
          height: nootkaWindow.fontSize * 20
          width: Math.min(parent.width * 0.9, nootkaWindow.fontSize * 26)
          anchors.horizontalCenter: parent.horizontalCenter
          clef: GLOB.clefType
          scoreObj.clefType: GLOB.clefType
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
      ButtonGroup { buttons: radioRow.children }
      Row {
        id: radioRow
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        TlabelText { text: qsTr("preferred accidentals:") }
        RadioButton { id: prefSharpRadio; text: qsTr("# - sharps"); checked: !GLOB.preferFlats }
        RadioButton { id: prefFlatRadio; text: qsTr("b - flats"); checked: GLOB.preferFlats }
      }
      description: qsTr("Choose which accidentals will be shown on the staff.")
    }

    Tile {
      visible: Noo.instr(instrSel.instrument).isGuitar
      TcheckBox {
        id: showOtherPosChB
        text: qsTr("show all possibilities of a note")
        anchors.horizontalCenter: parent.horizontalCenter
      }
      description: qsTr("As you know, the same note can be played in several places on the fingerboard.<br>If checked, all of them will be shown.")
    }

    Tile {
      visible: Noo.instr(instrSel.instrument).isGuitar
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("marked frets", "or frets with dots/marks"); anchors.verticalCenter: parent.verticalCenter }
        TextField {
          id: fretDots
          maximumLength: 30
          validator: RegExpValidator { regExp: /([1-2]{0,1}[0-9]{1,2}!{0,1},){0,10}/ }
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
    Tile {
      Transposition { id: transp }
    }
    Component.onCompleted: { // to avoid declaring every property signal in Tglobals.h
      showOtherPosChB.checked = GLOB.showOtherPos
      fretDots.text = GLOB.markedFrets
    }

  }

  function save() {
    GLOB.fingerColor = fingerColorButt.color
    GLOB.selectedColor = selectedColorButt.color
    GLOB.preferFlats = prefFlatRadio.checked
    GLOB.setInstrument(instrSel.instrument)
    if (Noo.instr(instrSel.instrument).isGuitar) {
      GLOB.setGuitarParams(fretsNrSpin.value, stringNrSpin.value)
      GLOB.showOtherPos = showOtherPosChB.checked
      GLOB.markedFrets = fretDots.text
    }
    GLOB.transposition = transp.outShift
    GLOB.audioInstrument = instrSel.instrument
    SOUND.acceptSettings()
  }

  function defaults() {
    fingerColorButt.color = Qt.rgba(1, 0, 0.5, 0.78)
    selectedColorButt.color = Qt.rgba(0.2, 0.6, 1.0, 1.0)
    GLOB.showOtherPos = false
    instrSel.instrument = Tinstrument.ClassicalGuitar // it will set transposition and preferred accidentals
    fretDots.text = "5,7,9,12!,15,19"
  }
}
