/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Grid {
  id: clefMenu
  columns: 1
  width: parent.width
  height: childrenRect.height
  spacing: Noo.fontSize() / 4
  horizontalItemAlignment: Grid.AlignHCenter

  property int selClef
  property int selId
  // private
  property var clefNr: [0, 1, 2, 4, 8, 32, 128]
  property var clefOff: [1.5, 0.8, 2.2, 1.5 , 0.8, 2.25, 0.8]

  signal clicked(var cl)

  onSelClefChanged: {
    for (var c = 0; c < 7; ++c) {
      clefButtons.itemAt(c).highlighted = (clefNr[c] === selClef)
      if (clefNr[c] === selClef)
        selId = c
    }
  }

  Component {
    id: clefPixComp
    Item {
      property int index: 0
      Text {
        id: clefStaff
        x: scale * 0.7
        font { family: "scorek"; pixelSize: 3 }
        text: "\ue014\ue014"
        color: activPal.text
      }
      Text {
        x: clefStaff.x + 0.3 * scale
        y: clefStaff.y - clefOff[index] * scale
        font { family: "scorek"; pixelSize: 3 }
        text: Noo.clef(clefNr[index]).glyph()
        color: activPal.text
      }
    }
  }

  Repeater {
    id: clefButtons
    model: 7
    Button {
      width: (clefMenu.width * 0.98) / clefMenu.columns
      height: Noo.fontSize() * (index === 6 ? 11.0 : 7.5)
      visible: index !== 0 || score.meter !== Tmeter.NoMeter
      onClicked: {
        selClef = clefNr[index]
        clefMenu.clicked(clefNr[index])
      }
      contentItem: Row {
        height: parent.height
        width: parent.width
        spacing: Noo.fontSize()
        Item { id: clefItem; width: Noo.fontSize() * 3.2; height: Noo.fontSize() }
        Column {
          anchors.verticalCenter: parent.verticalCenter
          width: parent.width - Noo.fontSize() * 4.0
          spacing: Noo.fontSize() / 4
          Text {
            antialiasing: true
            text: Noo.clef(clefNr[index]).name()
            font { bold: true; pixelSize: Noo.fontSize() * 1.1 }
            color: activPal.text
          }
          Text {
            antialiasing: true
            text: Noo.clef(clefNr[index]).desc()
            font { pixelSize: Noo.fontSize() * 0.8 }
            width: parent.width
            wrapMode: Text.WordWrap
            color: activPal.text
          }
        }
        Component.onCompleted:{
          clefPixComp.createObject(clefItem, {"scale": Noo.fontSize() * 0.9, "y": -2.2 * Noo.fontSize(), "index": index})
          if (index == 6) {
            var lowStaff = clefPixComp.createObject(clefItem, {"index": 2, "scale": Noo.fontSize() * 0.9, "y": Noo.fontSize() * 2.3})
            var brace = Qt.createQmlObject('import QtQuick 2.9; Text { font {family: "scorek"; pixelSize: 8 } text: "\ue000"; x: 0.15 }', lowStaff)
            brace.y = -10.0
            brace.color = activPal.text
          }
        }
      }
    }
  }
}
