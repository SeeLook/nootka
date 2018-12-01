/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  property alias instrument: instrSel.instrument

  height: parent.height
  contentHeight: instrCol.height
  contentWidth: width

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
      anchors.horizontalCenter: parent.horizontalCenter
      instrument: GLOB.instrument.type
    }
  }
}
