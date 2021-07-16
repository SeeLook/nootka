/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Window 2.12

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
    width: parent.width - tunerButt.width - NOO.factor()
    height: parent.height * 0.45
  }

  RectButton {
    id: tunerButt
    visible: volBar.visible
    x: parent.width - width - NOO.factor() / 2; y: parent.height * 0.04
    height: parent.height / 2
    enabled: !executor
    textColor: enabled ? activPal.text : disdPal.text
    text: GLOB.midAfreq + "Hz"
    font { pixelSize: parent.height * 0.36; bold: true }
    statusTip: qsTranslate("TempoBar", "Tuner")
    onClicked: {
      nootkaWindow.showDialog(7) // Nootka.Tuner
      SOUND.startListen()
    }
  }

  VolumeBar {
    id: volBar
    visible: !executor || executor.showPitchView
    active: pitchView.active
    y: parent.height * 0.55
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
