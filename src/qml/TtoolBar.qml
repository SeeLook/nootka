/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelapook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ToolBar {
  id: toolBar

  property alias scoreAct: scoreAct

  height: settAct.height
  background: Rectangle { anchors.fill: parent; color: activPal.window }

  Row {
    HeadButton { id: settAct; action: nootkaWindow.settingsAct }
    HeadButton { action: nootkaWindow.levelAct }
    HeadButton { id: scoreAct; action: nootkaWindow.scoreAct; visible: !GLOB.singleNoteMode }
    HeadButton { action: nootkaWindow.examAct }
  }

  Connections {
    target: SOUND
    property var pitchView
    onInitialized: {
      if (!pitchView) {
        var c = Qt.createComponent("qrc:/PitchView.qml")
        pitchView = c.createObject(toolBar)
        pitchView.x = Qt.binding(function() { return label.x - toolBar.width * 0.4 })
      }
    }
  }

  NootkaLabel {
    id: label
    anchors.right: parent.right
    height: toolBar.height
    onClicked: nootkaWindow.aboutAct.trigger()
  }

}
