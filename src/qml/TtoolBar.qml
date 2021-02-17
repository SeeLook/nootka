/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ToolBar {
  id: toolBar

  property alias scoreAct: scoreAct
  property alias melodyAct: melodyAct
  property alias examActions: examRepeater.model

  height: settAct.height
  background: TipRect { anchors.fill: parent; color: activPal.window; radius: 0 }
  z: 6
  topPadding: 0 // HACK: fixes QQC2 style which forces padding value

  Row {
    spacing: toolBar.width / 200
    HeadButton { id: settAct; taction: NOO.settingsAct; visible: !GLOB.isExam }
    HeadButton { taction: NOO.levelAct; visible: !GLOB.isExam }
    HeadButton { id: scoreAct; taction: NOO.scoreAct; visible: !GLOB.singleNoteMode }
    HeadButton { id: melodyAct; taction: NOO.melodyAct; visible: !GLOB.singleNoteMode && !GLOB.isExam }
    HeadButton { id: chartAct; taction: NOO.chartsAct; visible: !GLOB.isExam }
    HeadButton { taction: NOO.examAct; visible: !GLOB.isExam }
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
        pitchView.x = Qt.binding(function() { return (label.visible ? label.x : toolBar.width) - toolBar.width * 0.433 })
      }
    }
  }

  NootkaLabel {
    id: label
    visible: !executor
    anchors.right: parent.right
    height: toolBar.height
    onClicked: NOO.aboutAct.trigger()
  }
}
