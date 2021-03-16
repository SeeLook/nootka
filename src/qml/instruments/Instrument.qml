/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import "score"


Flickable {
  property var instrument: null
  property var score: null

  boundsBehavior: Flickable.StopAtBounds
  height: GLOB.instrument.getItemHeight(nootkaWindow.height)
  width: nootkaWindow.width * (GLOB.instrument.isSax ? 0.15 : 1)
  y: score.y + (GLOB.instrument.isSax ? 0 : score.height)
  x: GLOB.instrument.isSax ? parent.width - width : 0
  z: 1

  contentWidth: instrument ? instrument.width : 0
  contentHeight: instrument ? instrument.height : 0

  Component.onCompleted: setInstrument()

  Connections {
    target: GLOB
    onInstrumentChanged: setInstrument()
  }

  function setInstrument() {
    if (instrument)
      instrument.destroy()
    if (GLOB.instrument.type)
      instrument = Qt.createComponent("qrc:/instruments/" +  GLOB.instrument.qmlFile + ".qml").createObject(contentItem)
    else
      instrument = null
    if (GLOB.instrument.type === Tinstrument.Piano)
      instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
    NOO.instrument = instrument
  }

  Connections {
    target: score
    enabled: GLOB.instrument.type === Tinstrument.Piano
    onClefChanged: instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
  }
}
