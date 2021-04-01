/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Tflickable {
  id: clefPage

  height: parent.height
  contentHeight: mainItem.height
  contentWidth: width

  Item {
    id: mainItem
    y: Math.max(0, (clefPage.height - height) / 2)
    width: parent.width; height: childrenRect.height
  }

  function setInstrParams() {
    var selectedIns = instrPage.getInstrument()
    if (instrDetails && selectedIns === 0) {
        GLOB.clefType = instrDetails.clef
        GLOB.transposition = instrDetails.transposition
    } else {
        GLOB.clefType = NOO.instr(selectedIns).clef
        if (instrDetails && (selectedIns === 6 || selectedIns === 7)) // saxophones
          GLOB.transposition = instrDetails.transposition
        else
          GLOB.transposition = NOO.instr(selectedIns).transposition
    }

    var tuning
    if (selectedIns === 3) // bass
        tuning = NOO.tuning(100)
    else if (selectedIns === 1 || selectedIns === 2) // guitars
        tuning = NOO.tuning(0)
    else if (selectedIns === 0) {
        if (instrDetails)
          tuning = NOO.tuning(NOO.transpose(instrDetails.getNote(0), GLOB.transposition), NOO.transpose(instrDetails.getNote(1), GLOB.transposition),
                              NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote())
        else
          tuning = NOO.tuning(NOO.note(10), NOO.note(54), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote(), NOO.emptyNote())
    }
    GLOB.minSplitVol = GLOB.instrument.minSplitVol
    GLOB.skipStillerVal = GLOB.instrument.skipStillerVal
    if (GLOB.instrument.isGuitar)
      GLOB.setGuitarParams(GLOB.instrument.fretNumber, tuning)
    else if (selectedIns === 0)
      GLOB.setGuitarParams(0, tuning)
  }

  // private
  property var instrDetails: null

  function changeInstrument() {
    if (instrDetails !== null)
      instrDetails.destroy()

    var instrQML = "WizardOther";
    switch (nootkaWindow.instrument) {
      case 1:
      case 2:
        instrQML = "WizardGuitars"; break;
      case 3:
        instrQML = "WizardBass"; break;
      case 4:
        instrQML = "WizardPiano"; break;
      case 5:
        instrQML = "WizardBando"; break;
      case 6:
      case 7:
        instrQML = "WizardSax"; break;
    }
    instrDetails = Qt.createComponent("qrc:/wizard/" + instrQML +".qml").createObject(mainItem)
  }

  Connections {
    target: nootkaWindow
    onInstrumentChanged: changeInstrument()
  }
}
