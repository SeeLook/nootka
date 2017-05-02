/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Text {
  id: meter

  property bool readOnly: false
  property Drawer meterDrawer

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

  Component {
    id: meterComp
    Drawer { // meter menu
      visible: true
      width: nootkaWindow.width / 4; height: nootkaWindow.height

      Grid {
        width: parent.width
        anchors.margins: nootkaWindow.font.pixelSize / 2
        columns: 2
        spacing: nootkaWindow.font.pixelSize / 2

        Repeater {
          model: 12
          Button {
            height: nootkaWindow.font.pixelSize * 5
            width: parent.width / 2.1
            Text {
                id: buttText
                property int meter: Noo.meter(Math.pow(2, index)).meter()

                anchors.horizontalCenter: parent.horizontalCenter
                y: -nootkaWindow.font.pixelSize * 3.5
                font { family: "Scorek"; pixelSize: nootkaWindow.font.pixelSize * 4 }
                text: Noo.meter(meter).symbol()
                color: activPal.text
            }
            onClicked: {
                score.meter = buttText.meter
                meter.text = buttText.text
                meterDrawer.close()
            }
          }
        }
      }
    }
  }

  MouseArea {
      anchors.fill: parent
      enabled: !readOnly
      onClicked: {
        if (meterDrawer)
          meterDrawer.open()
        else 
          meterDrawer = meterComp.createObject(meter)
      }
  }
}
