/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Item {
  property var handleScore: null
  anchors.fill: parent
  MouseArea { // cover score mouse/touch actions
    anchors.fill: parent
    hoverEnabled: true
    onClicked: {
      handleScore = Qt.createComponent("qrc:/gotit/HandleScore.qml").createObject(nootkaWindow)
      handleScore.remaindChecked = GLOB.gotIt("howToScore", true)
      handleScore.closed.connect(gotItDone)
    }

    function gotItDone() {
      GLOB.setGotIt("howToScore", handleScore.remaindChecked)
      handleScore.destroy()
      destroy()
    }
  }
}
