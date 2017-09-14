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
    HeadButton { id: scoreAct; action: nootkaWindow.scoreAct }
    HeadButton { action: nootkaWindow.examAct }
  }


  PitchView {
    id: pitchView
    active: SOUND.listening
    anchors.right: label.left
    height: parent.height * 0.9
    width: parent.width * 0.4
    onPaused: {
      SOUND.stoppedByUser = !SOUND.stoppedByUser
      if (SOUND.listening)
        SOUND.stopListen()
      else
        SOUND.startListen()
    }
    Timer {
      repeat: true; interval: 75; running: SOUND.listening
      onTriggered: {
        pitchView.volume = SOUND.inputVol()
//         pitchView.deviation = SOUND.pitchDeviation()
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
