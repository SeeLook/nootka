/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


Tflickable {
  id: clefPage

  height: parent.height
  contentHeight: mainItem.height + NOO.factor() * 2; contentWidth: width

  Item {
    id: mainItem
    y: Math.max(0, (clefPage.height - height) / 2)
    width: parent.width; height: childrenRect.height
  }

  Component.onCompleted: changeInstrument()

  Connections {
    target: nootkaWindow
    onInstrumentChanged: changeInstrument()
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
      case 8:
        instrQML = "WizardUkulele"; break;
    }
    instrDetails = Qt.createComponent("qrc:/wizard/" + instrQML +".qml").createObject(mainItem)
  }

  function setInstrParams() {
    var selectedIns = instrPage.getInstrument()
    var instr = NOO.instr(selectedIns)
    if (instrDetails && instr.none) {
        GLOB.clefType = instrDetails.clef
        GLOB.transposition = instrDetails.transposition
    } else {
        GLOB.clefType = instr.clef
        if (instrDetails && instr.isSax) // saxophones
          GLOB.transposition = instrDetails.transposition
        else
          GLOB.transposition = instr.transposition
    }

    var tuning
    var empty = NOO.emptyNote()
    if (instr.none && instrDetails)
      tuning = NOO.tuning(instrDetails.getNote(0), instrDetails.getNote(1), empty, empty, empty, empty)
    else
      tuning = NOO.defaultScale(selectedIns)
    if (instr.isGuitar)
      GLOB.markedFrets = instr.ukulele ? "5,7,10,12!" : "5,7,9,12!,15,17"

    GLOB.setGuitarParams(instr.fretNumber, tuning)
  }
}
