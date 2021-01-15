/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  color: meterArea.containsMouse && scoreObj.editMode ? GLOB.noteCursorColor : activPal.text
  text: Noo.meter(score.meter).symbol()
  y: score.upperLine - 11
  x: (staff0.keySignItem ? staff0.keySignItem.x + staff0.keySignItem.width : staff0.clef.x + staff0.clef.width) + 1.0

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerMeter : null }
  Component {
    id: lowerMeter
    Text {
      font: meter.font
      color: meter.color
      text: meter.text
      y: 22
    }
  }

  MouseArea {
    id: meterArea
    y: 6
    width: parent.width; height: parent.height + (score.clef === Tclef.PianoStaffClefs ? 2 : -20)
    enabled: !score.readOnly && scoreObj.editMode
    hoverEnabled: true
    property Drawer meterDrawer
    onClicked: {
      if (meterDrawer)
        meterDrawer.open()
      else
        meterDrawer = Qt.createComponent("qrc:/score/MeterDrawer.qml").createObject(meter)
    }
  }
}
