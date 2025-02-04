/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka 1.0
import "../"


Tflickable {
  contentHeight: phoneCol.height
  contentWidth: width

  Column {
    id: phoneCol
    width: parent.width
    topPadding: NOO.factor()

    Tile {
      width: parent.width * 0.98
      Column {
        spacing: NOO.factor() * 1.5
        anchors.horizontalCenter: parent.horizontalCenter

        TcheckBox {
          id: screenOnChB
          text: qsTr("screen always on")
          checked: GLOB.isKeepScreenOn()
        }
        TcheckBox {
          visible: NOO.shortScreenSide() / NOO.fingerPixels() >= 12 // 8,4 cm width, 7" screens
          enabled: !NOO.instr(settings.instrument).isSax
          id: disRotatChB
          text: qsTr("disable screen rotation")
          checked: GLOB.disableRotation()
        }
        TcheckBox {
          id: fullScrChB
          text: qsTr("full screen")
          checked: GLOB.fullScreen()
        }
      }
    }
    Tile {
      width: parent.width * 0.98
      description: qsTr("Touching a screen suspends pitch detection for 1 second but some advanced users may want to disable that.")
      TcheckBox {
        id: touchSniffChB
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("suspend mic when screen was touched")
        checked: GLOB.touchStopsSniff()
      }
    }

  }

  function save() {
    GLOB.keepScreenOn(screenOnChB.checked)
    GLOB.setDisableRotation(disRotatChB.enabled && disRotatChB.checked)
    nootkaWindow.visibility = fullScrChB.checked ? "FullScreen" : "AutomaticVisibility"
    GLOB.setFullScreen(fullScrChB.checked)
    SOUND.setTouchHandling(touchSniffChB.checked)
  }

  function defaults() {
    screenOnChB.checked = true
    disRotatChB.checked = true
    fullScrChB.checked = true
    touchSniffChB.checked = true
  }
}


