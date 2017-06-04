/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Nootka 1.0


Item {
  id: root
  property alias instrument: instrLoad.item

  signal note(var n)

  height: GLOB.instrument.type === Tinstrument.Piano ? Math.min(nootkaWindow.height / 5, nootkaWindow.fontSize * 12) : nootkaWindow.height / 4
  width: nootkaWindow.width

  Loader {
    id: instrLoad
    anchors.fill: parent
    source: "qrc:/" +  (GLOB.instrument.type === Tinstrument.Piano ? "Piano" : "Guitar") + ".qml"
  }

  Connections {
    target: instrLoad.item
    onNoteChanged: root.note(instrument.note)
  }
}
