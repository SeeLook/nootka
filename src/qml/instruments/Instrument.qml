/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import "score"


Item {
  property alias instrument: instrLoad.item
  property Score score

  height: GLOB.instrument.getItemHeight(nootkaWindow.height)
  width: nootkaWindow.width * (GLOB.instrument.isSax ? 0.15 : 1)
  y: score.y + (GLOB.instrument.isSax ? 0 : score.height)
  x: GLOB.instrument.isSax ? parent.width - width : 0
  z: 1

  Loader {
    id: instrLoad
    anchors.fill: parent
    source: GLOB.instrument.type ? "qrc:/instruments/" +  GLOB.instrument.qmlFile + ".qml" : ""
    onLoaded: {
      if (GLOB.instrument.type === Tinstrument.Piano)
        instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
    }
  }

  onInstrumentChanged: Noo.instrument = instrument

  Connections {
    target: score
    enabled: GLOB.instrument.type === Tinstrument.Piano
    onClefChanged: instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
  }
}
