/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelapook@gmail.com)   *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ToolBar {
  id: toolBar

  property alias scoreAct: scoreAct
  property alias melodyAct: melodyAct
  property alias examActions: examRepeater.model

  height: settAct.height
  background: TipRect { anchors.fill: parent; color: activPal.window; radius: 0; verticalOffset: toolBar.height / 12; horizontalOffset: 0 }

  Row {
    spacing: toolBar.width / 200
    HeadButton { id: settAct; taction: Noo.settingsAct; visible: !GLOB.isExam }
    HeadButton { taction: Noo.levelAct; visible: !GLOB.isExam }
    HeadButton { id: scoreAct; taction: Noo.scoreAct; visible: !GLOB.singleNoteMode }
    HeadButton { id: melodyAct; taction: Noo.melodyAct; visible: !GLOB.singleNoteMode && !GLOB.isExam }
    HeadButton { id: chartAct; taction: Noo.chartsAct; visible: !GLOB.isExam }
    HeadButton { taction: Noo.examAct; visible: !GLOB.isExam }
    Repeater {
      id: examRepeater
      HeadButton { taction: modelData; visible: taction && taction.enabled }
    }
  }

  Connections {
    target: SOUND
    property var pitchView
    onInitialized: {
      if (!pitchView) {
        var c = Qt.createComponent("qrc:/PitchView.qml")
        pitchView = c.createObject(toolBar)
        pitchView.x = Qt.binding(function() { return label.x - toolBar.width * 0.403 })
      }
    }
  }

  NootkaLabel {
    id: label
    anchors.right: parent.right
    height: toolBar.height
    onClicked: Noo.aboutAct.trigger()
  }

}
