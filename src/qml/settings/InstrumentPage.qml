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
  contentHeight: instrCol.height + Noo.fontSize() * 2
  contentWidth: Math.max(width, Noo.fontSize() * 35)

  property bool first: true // read props first time from GLOB but when instrument changed then from its profile

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

  Column {
    id: instrCol
    width: parent.width
    spacing: Noo.fontSize() / 2

    Tile {
      description: qsTr("Instrument")
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
              tuningCombo.model = instrument === Tinstrument.BassGuitar ? Noo.bassTunings() : Noo.guitarTunings()
              if (ins.isGuitar) {
                score.clef = ins.clef
                if (instrument === Tinstrument.BassGuitar)
                  setTuning(Noo.tuning(Ttune.Bass4_EADG))
                else
                  setTuning(Noo.tuning(Ttune.Standard_EADGBE))
              }
          }
        }
      }
    }

    Grid {
      visible: Noo.instr(instrSel.instrument).isGuitar
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize()
      columns: parent.width < Noo.fontSize() * 50 ? 1 : 2
      horizontalItemAlignment: Grid.AlignHCenter
      Row {
        spacing: Noo.fontSize()
        Text { text: qsTr("number of frets:"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
        SpinBox { id: fretsNrSpin; from: 15; to: 24; value: GLOB.fretNumber }
      }
      Row {
        spacing: Noo.fontSize()
        Text { text: qsTr("number of strings:"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
        SpinBox { id: stringNrSpin; from: 3; to: 6; value: GLOB.stringNumber() }
      }
    }

    Tile {
      visible: instrSel.instrument <= Tinstrument.BassGuitar
      Column {
        spacing: Noo.fontSize() / 4
        width: parent.width
        Row {
          spacing: Noo.fontSize()
          anchors.horizontalCenter: parent.horizontalCenter
          Text { text: qsTr("tuning of the guitar"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
          ComboBox {
            id: tuningCombo
            width: Noo.fontSize() * 16
            model: GLOB.instrument.type === Tinstrument.BassGuitar ? Noo.bassTunings() : Noo.guitarTunings()
            delegate: ItemDelegate { text: modelData }
          }
        }
        Score {
          id: score
          height: Noo.fontSize() * 20
          width: Math.min(parent.width * 0.9, Noo.fontSize() * 26)
          anchors.horizontalCenter: parent.horizontalCenter
          clef: GLOB.clefType
          meter: Tmeter.NoMeter
          scoreObj.onClicked: tuningCombo.currentIndex = tuningCombo.count - 1
          Component.onCompleted: {
            for (var s = 1; s <= GLOB.tuning.stringNumber; ++s)
              score.addNote(GLOB.tuning.string(s))
            stringNrSpin.valueModified.connect(strNrChanged)
            tuningCombo.activated.connect(tuningSelected)
          }
          function strNrChanged() {
            if (stringNrSpin.value > score.notesCount) {
                while (stringNrSpin.value > score.notesCount)
                  score.addNote(Noo.note(0, 0, 0, 0))
                tuningCombo.currentIndex = tuningCombo.count - 1
            } else if (stringNrSpin.value < score.notesCount) {
                while (stringNrSpin.value < score.notesCount)
                  score.deleteLast()
                tuningCombo.currentIndex = tuningCombo.count - 1
            }
          }
          function tuningSelected() {
            if (tuningCombo.currentIndex < tuningCombo.count - 1)
              setTuning(Noo.tuning((instrSel.instrument === Tinstrument.BassGuitar ? 100 : 0) + tuningCombo.currentIndex))
          }
        }
      }
      description: qsTr("Select appropriate tuning from the list or prepare your own.") + "<br>" + 
                    qsTr("Remember to select the appropriate clef in Score settings.")
    }

    Tile {
      Transposition { id: transp }
      description: qsTr("Difference between score notation and real sound pitch.")
    }

    Tile {
      ButtonGroup { buttons: radioRow.children }
      Row {
        id: radioRow
        spacing: Noo.fontSize()
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
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("marked frets", "or frets with dots/marks"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
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
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a pointer on an instrument"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: fingerColorButt; color: GLOB.fingerColor }
      }
    }
    Tile {
      Row {
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a selection"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: selectedColorButt; color: GLOB.selectedColor }
      }
    }
    Component.onCompleted: { // to avoid declaring every property signal in Tglobals.h
      showOtherPosChB.checked = GLOB.showOtherPos
      fretDots.text = GLOB.markedFrets
      if (GLOB.instrument.isGuitar) {
        if (GLOB.tuning.type === Ttune.Custom)
          tuningCombo.currentIndex = tuningCombo.count - 1
        else
          tuningCombo.currentIndex = GLOB.tuning.type - (GLOB.instrument.type === Tinstrument.BassGuitar ? 100 : 0)
      }
    }

  }

  function setTuning(t) {
    for (var i = 0; i < 6; ++i) {
      if (i < t.stringNr()) {
          if (i >= score.notesCount)
            score.addNote(t.str(i + 1))
          else
            score.setNote(score.scoreObj.note(i), t.str(i + 1))
      } else {
          if (score.notesCount > t.stringNr())
            score.deleteLast()
      }
    }
  }

  function save() {
    GLOB.fingerColor = fingerColorButt.color
    GLOB.selectedColor = selectedColorButt.color
    GLOB.preferFlats = prefFlatRadio.checked
    GLOB.setInstrument(instrSel.instrument)
    GLOB.transposition = transp.outShift
    if (Noo.instr(instrSel.instrument).isGuitar) {
      GLOB.showOtherPos = showOtherPosChB.checked
      GLOB.markedFrets = fretDots.text
      var tun
      if (tuningCombo.currentIndex === tuningCombo.count - 1)
          tun = Noo.tuning(score.scoreObj.noteAt(0), score.scoreObj.noteAt(1), score.scoreObj.noteAt(2),
                           score.scoreObj.noteAt(3), score.scoreObj.noteAt(4), score.scoreObj.noteAt(5))
      else {
          if (instrSel.instrument !== Tinstrument.BassGuitar && tuningCombo.currentIndex === 0)
            tun = Noo.tuning(Ttune.Standard_EADGBE)
          else
            tun = Noo.tuning(tuningCombo.currentIndex + (instrSel.instrument === Tinstrument.BassGuitar ? 100 : 0))
      }
      GLOB.setGuitarParams(fretsNrSpin.value, tun) // TODO left-handed guitar
    }
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

  function help() { Noo.openHelpLink("instrument-settings") }
}
