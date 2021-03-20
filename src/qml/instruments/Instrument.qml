/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import "score"


Flickable {
  id: instrFlick

  property var instrument: null
  property var score: null

  boundsBehavior: Flickable.StopAtBounds
  height: sizable.height
  width: nootkaWindow.width * (GLOB.instrument.isSax ? 0.15 : 1)
  y: GLOB.instrument.isSax ? 0 : nootkaWindow.contentItem.height - height
  x: GLOB.instrument.isSax ? parent.width - width : 0

  z: instrument && instrument.scale > 1.0 ? 7 : 1

  contentWidth: sizable.width
  contentHeight: sizable.height

  Item {
    id: sizable
    width: instrument ? instrument.width * instrument.scale : 0
    height: instrument ? instrument.height * instrument.scale : 0
  }

  Component.onCompleted: setInstrument()

  Connections {
    target: GLOB
    onInstrumentChanged: setInstrument()
  }

  property real hiFactor: GLOB.instrument.getItemHeight(100) / 100.0
  function setInstrument() {
    if (instrument)
      instrument.destroy()
    if (GLOB.instrument.type)
      instrument = Qt.createComponent("qrc:/instruments/" +  GLOB.instrument.qmlFile + ".qml").createObject(sizable)
    else
      instrument = null
    if (GLOB.instrument.piano)
      instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
    NOO.instrument = instrument
    if (instrument && !GLOB.instrument.isSax)
      score.parent.scale = Qt.binding(function() { return (1.0 - hiFactor * instrument.scale) / (1 - hiFactor) })
  }

  Connections {
    target: score
    enabled: GLOB.instrument.piano
    onClefChanged: instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
  }
}
