/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  id: instrPage
  property int instrument: -1

  height: parent.height
  contentHeight: instrCol.height
  contentWidth: width

  Tile {
    y: (instrPage.height - height) / 2
    bgColor: Qt.tint(nootkaWindow.labelColor, Noo.alpha(activPal.base, 230))
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Column {
      id: instrCol
      width: parent.width
      spacing: Noo.fontSize() * 3
      Text {
        text: qsTr("What instrument do you play?")
        font { pixelSize: Noo.fontSize() * 2; bold: true }
        width: parent.width
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        color: activPal.text
      }
      InstrumentSelector {
        id: instrSel
        height: nootkaWindow.width / 7
        anchors.horizontalCenter: parent.horizontalCenter
        instrument: GLOB.instrument.type
      }
    }
  }

  function getInstrument() { return instrSel.instrument }

  Connections {
    target: nootkaWindow.swipe
    onCurrentIndexChanged: {
      if (nootkaWindow.swipe.currentIndex != 1 && instrPage.instrument != instrSel.instrument)
        instrPage.instrument = instrSel.instrument
    }
  }
}
