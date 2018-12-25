/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Tflickable {
  id: wizardOpts

  height: parent.height
  contentWidth: width

  function setOptions() {
    if (optContext) {
      GLOB.seventhIsB = optContext.is7B
      GLOB.enableDoubleAccids = optContext.doubleAccids
      GLOB.keySignatureEnabled = optContext.useKeys
      GLOB.showEnharmNotes = optContext.enharmNames
      GLOB.noteNameStyle = optContext.style
    }
  }

  // private
  property var optContext: null

  Connections {
    target: nootkaWindow.swipe
    onCurrentIndexChanged: {
      if (nootkaWindow.swipe.currentIndex == 3 && !optContext) {
        var c = Qt.createComponent("qrc:/wizard/OptionsContext.qml")
        optContext = c.createObject(wizardOpts.contentItem)
        wizardOpts.contentHeight = optContext.height
      }
    }
  }
}
