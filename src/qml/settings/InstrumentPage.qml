/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import Nootka 1.0
import "../"
import "../score"


Flickable {
  id: instrPage

  clip: true
  width: parent.width
  height: parent.height
  contentHeight: instrCol.height + NOO.factor() * 2
  contentWidth: Math.max(width, NOO.factor() * 35)

  // private
  property bool first: true // read props first time from GLOB but when instrument changes then from its profile
  property int initInstr: -1

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

  Column {
    id: instrCol
    width: parent.width
    spacing: NOO.factor() / 2

    Tile {
      description: qsTr("Instrument")
      InstrumentSelector {
        id: instrSel
        anchors.horizontalCenter: parent.horizontalCenter
        onInstrumentChanged: { // load instrument profile
          if (first) {
              transp.shift = GLOB.transposition
              prefFlatRadio.checked = GLOB.preferFlats
          } else {
              settings.instrument = instrument
              var ins = NOO.instr(instrument)
              settings,clef = ins.clef
              transp.shift = ins.transposition
              prefFlatRadio.checked = ins.isSax ? true : false
              prefSharpRadio.checked = ins.isSax ? false : true
              tuningCombo.model = instrument === Tinstrument.BassGuitar ? NOO.bassTunings() : NOO.guitarTunings()
              if (ins.isGuitar) {
                  if (instrument === Tinstrument.BassGuitar)
                    setTuning(NOO.tuning(Ttune.Bass4_EADG))
                  else
                    setTuning(NOO.tuning(Ttune.Standard_EADGBE))
                  fretsNrSpin.value = ins.fretNumber
                  tuningCombo.currentIndex = 0
                  if (fretDots.text === "")
                    fretDots.text = "5,7,9,12!,15,17"
              } else if (ins.none) {
                  setTuning(NOO.tuning(score.scoreObj.lowestNote(), score.scoreObj.highestNote(), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote()))
              }
          }
        }
      }
    }

    Tile {
      visible: instrSel.instrument <= Tinstrument.BassGuitar
      Column {
        spacing: NOO.factor()
        width: parent.width
        Grid {
          visible: NOO.instr(instrSel.instrument).isGuitar
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: NOO.factor()
          columns: parent.width < NOO.factor() * 50 ? 1 : 2
          horizontalItemAlignment: Grid.AlignHCenter
          Row {
            spacing: NOO.factor()
            Text { text: qsTr("number of frets:"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
            TspinBox { id: fretsNrSpin; from: 15; to: 24; value: GLOB.fretNumber }
          }
          Row {
            spacing: NOO.factor()
            Text { text: qsTr("number of strings:"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
            TspinBox { id: stringNrSpin; from: 3; to: 6; value: GLOB.stringNumber() }
          }
        }
        Item { width: NOO.factor(); height: NOO.factor() }
        Row {
          spacing: NOO.factor()
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            text: instrSel.instrument === 0 ? qsTr("scale of an instrument") : qsTr("tuning of the guitar")
            anchors.verticalCenter: parent.verticalCenter; color: activPal.text
          }
          TcomboBox {
            id: tuningCombo
            visible: instrSel.instrument !== 0
            width: NOO.factor() * 18
            model: GLOB.instrument.bassGuitar ? NOO.bassTunings() : NOO.guitarTunings()
          }
        }
        GlowRect {
          height: score.height; width: score.width +  NOO.factor(); radius: NOO.factor() / 2
          color: activPal.base
          anchors.horizontalCenter: parent.horizontalCenter
          Score {
            id: score
            anchors.horizontalCenter: parent.horizontalCenter
            height: NOO.factor() * 18
            width: Math.min(instrPage.width * 0.9, NOO.factor() * (instrSel.instrument === 0 ? 14 : 28))
            bgColor: "transparent"
            meter: Tmeter.NoMeter
            scoreObj.onClicked: tuningCombo.currentIndex = tuningCombo.count - 1
            scoreObj.editMode: true
            clef: settings.clef
            onClefChanged: settings.clef = clef
            Component.onCompleted: {
              stringNrSpin.valueModified.connect(strNrChanged)
              tuningCombo.activated.connect(tuningSelected)
            }
            function strNrChanged() {
              if (stringNrSpin.value > score.notesCount) {
                  while (stringNrSpin.value > score.notesCount) {
                    score.addNote(NOO.note(0, 0, 0, 0))
                    score.scoreObj.lastNote.setStringNumber(score.notesCount)
                  }
                  tuningCombo.currentIndex = tuningCombo.count - 1
              } else if (stringNrSpin.value < score.notesCount) {
                  while (stringNrSpin.value < score.notesCount)
                    score.deleteLast()
                  tuningCombo.currentIndex = tuningCombo.count - 1
              }
            }
            function tuningSelected() {
              if (tuningCombo.currentIndex < tuningCombo.count - 1)
                setTuning(NOO.tuning((instrSel.instrument === Tinstrument.BassGuitar ? 100 : 0) + tuningCombo.currentIndex))
            }
          }
        }
      }
      description: instrSel.instrument === 0 ? "" : qsTr("Select appropriate tuning from the list or prepare your own.") + "<br>"
                    + qsTr("Remember to select the appropriate clef in Score settings.")
    }

    Tile {
      Transposition { id: transp }
      description: qsTr("Difference between score notation and real sound pitch.")
    }

    Tile {
      ButtonGroup { buttons: radioRow.children }
      Row {
        id: radioRow
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TlabelText { text: qsTr("preferred accidentals:") }
        TradioButton { id: prefSharpRadio; text: qsTr("# - sharps"); checked: !GLOB.preferFlats }
        TradioButton { id: prefFlatRadio; text: qsTr("b - flats"); checked: GLOB.preferFlats }
      }
      description: qsTr("Choose which accidentals will be shown on the staff.")
    }

    Tile {
      visible: NOO.instr(instrSel.instrument).isGuitar
      TcheckBox {
        id: showOtherPosChB
        text: qsTr("show all possibilities of a note")
        anchors.horizontalCenter: parent.horizontalCenter
      }
      description: qsTr("As you know, the same note can be played in several places on the fingerboard.<br>If checked, all of them will be shown.")
    }

    Tile {
      visible: NOO.instr(instrSel.instrument).isGuitar
      Row {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("marked frets", "or frets with dots/marks"); anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
        TtextField {
          id: fretDots
          width: NOO.factor() * 15
          maximumLength: 30
          validator: RegExpValidator { regExp: /([1-2]{0,1}[0-9]{1,2}!{0,1},){0,10}/ }
        }
      }
      description: qsTr("Put numbers of frets marked with dot. Separate the numbers with comma. Add ! (exclamation mark) after a number to paint a dot twice.")
    }

    Tile {
      visible: instrSel.instrument !== 0
      Row {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a pointer on an instrument"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: fingerColorButt; color: GLOB.fingerColor; title: qsTr("color of a pointer on an instrument") }
      }
    }
    Tile {
      visible: instrSel.instrument !== 0
      Row {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        Text { color: activPal.text; text: qsTr("color of a selection"); anchors.verticalCenter: parent.verticalCenter }
        ColorButton { id: selectedColorButt; color: GLOB.selectedColor; title: qsTr("color of a selection") }
      }
    }

  }

  Timer { // workaround
    running: true
    interval: 50
    onTriggered: {
      initInstr = GLOB.instrument.type
      showOtherPosChB.checked = GLOB.showOtherPos
      fretDots.text = GLOB.markedFrets
      var tmpTrans = GLOB.transposition
      instrSel.instrument = GLOB.instrument.type
      transp.shift = tmpTrans
      if (GLOB.instrument.isGuitar) {
          if (GLOB.tuning.type === Ttune.Custom) {
              setTuning(NOO.tuning(GLOB.tuning.string(1), GLOB.tuning.string(2), GLOB.tuning.string(3),
                                  GLOB.tuning.string(4), GLOB.tuning.string(5), GLOB.tuning.string(6)))
              tuningCombo.currentIndex = tuningCombo.count - 1
          } else {
              tuningCombo.currentIndex = GLOB.tuning.type - (GLOB.instrument.bassGuitar ? 100 : 0)
              setTuning(NOO.tuning(GLOB.tuning.type))
          }
      } else {
          setTuning(NOO.tuning(GLOB.tuning.string(1), GLOB.tuning.string(2),
                      NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote()))
      }
      first = false
    }
  }

  function setTuning(t) {
    for (var i = 0; i < 6; ++i) {
      if (i < t.stringNumber) {
          if (i >= score.notesCount)
            score.addNote(t.str(i + 1))
          else
            score.setNote(i, t.str(i + 1))
          score.scoreObj.note(i).setStringNumber(NOO.instr(instrSel.instrument).isGuitar ? i + 1 : 0)
      } else {
          if (score.notesCount > t.stringNr())
            score.deleteLast()
      }
    }
    stringNrSpin.value = t.stringNumber
  }

  function save() {
    GLOB.fingerColor = fingerColorButt.color
    GLOB.selectedColor = selectedColorButt.color
    GLOB.preferFlats = prefFlatRadio.checked
    GLOB.transposition = transp.outShift
    var instrChanged = instrSel.instrument !== initInstr
    if (NOO.instr(instrSel.instrument).isGuitar) {
        GLOB.showOtherPos = showOtherPosChB.checked
        GLOB.markedFrets = fretDots.text
        var tun
        if (tuningCombo.currentIndex === tuningCombo.count - 1)
            tun = NOO.tuning(score.scoreObj.noteAt(0), score.scoreObj.noteAt(1), score.scoreObj.noteAt(2),
                            score.scoreObj.noteAt(3), score.scoreObj.noteAt(4), score.scoreObj.noteAt(5))
        else {
            if (instrSel.instrument !== Tinstrument.BassGuitar && tuningCombo.currentIndex === 0)
              tun = NOO.tuning(Ttune.Standard_EADGBE)
            else
              tun = NOO.tuning(tuningCombo.currentIndex + (instrSel.instrument === Tinstrument.BassGuitar ? 100 : 0))
        }
        // TODO left-handed guitar
        // HACK: when instrument changed, set default tuning at first, then real tuning will show scordature, if any
        GLOB.setGuitarParams(fretsNrSpin.value, instrChanged ? NOO.tuning(Ttune.Standard_EADGBE) : tun)
        GLOB.setInstrument(instrSel.instrument)
        if (instrChanged)
          GLOB.setGuitarParams(fretsNrSpin.value, tun)
    } else {
        GLOB.setInstrument(instrSel.instrument)
        GLOB.setGuitarParams(0, NOO.defaultScale(instrSel.instrument))
    }
    GLOB.audioInstrument = instrSel.instrument
    SOUND.acceptSettings()
  }

  function defaults() {
    fingerColorButt.color = Qt.rgba(1, 0, 0.5, 0.78)
    selectedColorButt.color = Qt.rgba(0.2, 0.6, 1.0, 1.0)
    GLOB.showOtherPos = false
    instrSel.instrument = initInstr === 0 ? 7 : 0
    instrSel.instrument = initInstr // switch instrument twice to load its defaults
    fretDots.text = "5,7,9,12!,15,17"
  }

  function help() { NOO.openHelpLink("instrument-settings") }
}
