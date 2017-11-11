/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Text {
  id: meter

  Connections {
      target: score
      onMeterChanged: text = Noo.meter(score.meter).symbol()
  }

  font { family: "Scorek"; pixelSize: 8 }
  color: activPal.text
  text: Noo.meter(score.meter).symbol()
  y: score.upperLine - 9
  x: (parent.keySignature ? parent.keySignature.x + parent.keySignature.width : 0.5 + parent.clef.width) + 1.0

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerMeter : null }
  Component {
      id: lowerMeter
      Text {
        font: meter.font
        color: meter.color
        text: meter.text
        y: 14
      }
  }

  MouseArea {
      anchors.fill: parent
      enabled: !score.readOnly
      property Drawer meterDrawer
      onClicked: {
        if (meterDrawer)
            meterDrawer.open()
        else {
            var c = Qt.createComponent("qrc:/MeterDrawer.qml")
            meterDrawer = c.createObject(meter)
        }
      }
  }
}
