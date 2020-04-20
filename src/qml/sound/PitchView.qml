/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2

import "sound"


Item {
  id: pitchView

  property alias volume : volBar.volume
  property bool active: SOUND.listening && !SOUND.tunerMode

  signal paused()

  height: parent.height * 0.9
  width: parent.width * 0.43

  // protected
  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.4

  TempoBar {
    id: tempoBar
    visible: !GLOB.singleNoteMode && ((GLOB.rhythmsEnabled && !executor) || (executor && executor.showRtmView))
    y: parent.height * 0.05
    width: parent.width
    height: parent.height * 0.45
  }

  VolumeBar {
    id: volBar
    visible: !executor || executor.showPitchView
    active: pitchView.active
    y: parent.height * (tempoBar.visible ? 0.55 : 0.27)
    width: parent.width - micButt.width * 2
    height: parent.height * 0.45
    Shortcut { sequence: "M"; onActivated: pitchView.paused(); enabled: !executor || executor.showPitchView }
    RectButton {
      id: micButt
      statusTip: qsTr("Start/stop pitch detection") + "<br><b>(M)</b>"
      x: volBar.width + width * 0.5
      font { family: "Nootka"; pixelSize: volBar.height }
      text: "r"
      onClicked: pitchView.paused()
      checked: SOUND.listening
    }
  }

  onPaused: {
    SOUND.stoppedByUser = !SOUND.stoppedByUser
    if (SOUND.listening)
      SOUND.stopListen()
    else
      SOUND.startListen(false)
  }

  Timer {
    repeat: true; interval: 75; running: pitchView.active
    onTriggered: volume = SOUND.inputVol()
  }

}
