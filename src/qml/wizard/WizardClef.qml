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
    if (instrDetails && nootkaWindow.instrument === 0) {
        GLOB.clefType = instrDetails.clef
        GLOB.transposition = instrDetails.transposition
    } else {
        GLOB.clefType = Noo.instr(nootkaWindow.instrument).clef
        GLOB.transposition = Noo.instr(nootkaWindow.instrument).transposition
    }
  }

  // private
  property var instrDetails: null

  function changeInstrument() {
    if (instrDetails !== null)
      instrDetails.destroy()

    if (nootkaWindow.instrument === 0) {
        var c = Qt.createComponent("qrc:/wizard/WizardOther.qml")
        instrDetails = c.createObject(mainItem)
    } else if (nootkaWindow.instrument === 1 || nootkaWindow.instrument === 2) {
        var c = Qt.createComponent("qrc:/wizard/WizardGuitars.qml")
        instrDetails = c.createObject(mainItem)
    } else if (nootkaWindow.instrument === 3) {
        var c = Qt.createComponent("qrc:/wizard/WizardBass.qml")
        instrDetails = c.createObject(mainItem)
    } else if (nootkaWindow.instrument === 4) {
        var c = Qt.createComponent("qrc:/wizard/WizardPiano.qml")
        instrDetails = c.createObject(mainItem)
    } else if (nootkaWindow.instrument === 5) {
        var c = Qt.createComponent("qrc:/wizard/WizardBando.qml")
        instrDetails = c.createObject(mainItem)
    } else if (nootkaWindow.instrument === 6 || nootkaWindow.instrument === 7) {
        var c = Qt.createComponent("qrc:/wizard/WizardSax.qml")
        instrDetails = c.createObject(mainItem)
    }
  }

  Connections {
    target: nootkaWindow
    onInstrumentChanged: changeInstrument()
  }
}
