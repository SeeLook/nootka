/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

Item {
  id: root
  property Item instrument: null

  signal note(var n)

  height: nootkaWindow.height / 5 //4
  width: nootkaWindow.width

  Component.onCompleted: {
    var c = Qt.createComponent("qrc:/Piano.qml")
    instrument = c.createObject(root, {"anchors.fill": root})
    instrument.onNoteChanged.connect(callNote)
  }

  function callNote() { root.note(instrument.note) }
}
