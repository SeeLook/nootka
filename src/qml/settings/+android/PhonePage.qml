/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */


import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  contentHeight: phoneCol.height
  contentWidth: width

  Column {
    id: phoneCol
    anchors.horizontalCenter: parent.horizontalCenter
    topPadding: Noo.fontSize()
    spacing: Noo.fontSize()

    TcheckBox {
      id: screenOnChB
      text: qsTr("keep screen on")
      checked: GLOB.isKeepScreenOn()
    }
    TcheckBox {
      id: disRotatChB
      text: qsTr("disable screen rotation")
      checked: GLOB.disableRotation()
    }
    TcheckBox {
      id: fullScrChB
      text: qsTr("use full screen")
      checked: GLOB.fullScreen()
    }

  }

  function save() {
    GLOB.keepScreenOn(screenOnChB.checked)
    GLOB.setDisableRotation(disRotatChB.checked)
    nootkaWindow.visibility = fullScrChB.checked ? "FullScreen" : "AutomaticVisibility"
    GLOB.setFullScreen(fullScrChB.checked)
  }

  function defaults() {
    screenOnChB.checked = true
    disRotatChB.checked = true
    fullScrChB.checked = true
  }
}


