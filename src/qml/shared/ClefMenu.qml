/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


ListView {
  id: clefMenu
  width: parent.width; height: parent.height
  clip: true
  spacing: Noo.fontSize() / 2
  cacheBuffer: Noo.fontSize() * 56

  property int selClef
  // private
  property var clefNr: [0, 1, 2, 4, 8, 32, 128]
  property var clefOff: [1.5, 0.8, 2.2, 1.5 , 0.8, 2.25, 0.8]
  property var clefButtons: []

  signal clicked(var cl)

  function checkClef() {
    for (var c = 0; c < clefButtons.length; ++c) {
      if (clefNr[c] === selClef)
        currentIndex = c
    }
  }

  onSelClefChanged: checkClef()

  onVisibleChanged: {
    if (visible)
      checkClef()
  }

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

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

  model: 7
  delegate: TcuteButton {
    id: clefButt
    x: Noo.fontSize() / 4
    width: (clefMenu.width - Noo.fontSize() / 2) // / clefMenu.columns
    height: Noo.fontSize() * (index === 6 ? 10 : 7.5)
    visible: index !== 0 || score.meter !== Tmeter.NoMeter
    checked: clefNr[index] === selClef
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
          text: Noo.clef(clefNr[index]).name()
          font { bold: true; pixelSize: Noo.fontSize() * 1.1 }
          color: activPal.text
        }
        Text {
          padding: 4
          text: Noo.clef(clefNr[index]).desc()
          font { pixelSize: Noo.fontSize() * 0.8 }
          width: parent.width
          wrapMode: Text.WordWrap
          color: activPal.text
        }
      }
      Component.onCompleted: {
        clefPixComp.createObject(clefItem, {"scale": Noo.fontSize() * 0.9, "y": -1.5 * Noo.fontSize(), "index": index})
        if (index == 6) {
          var lowStaff = clefPixComp.createObject(clefItem, {"index": 2, "scale": Noo.fontSize() * 0.9, "y": Noo.fontSize() * 2.9})
          var brace = Qt.createQmlObject('import QtQuick 2.9; Text { font {family: "scorek"; pixelSize: 8 } text: "\ue000"; x: 0.15 }', lowStaff)
          brace.y = -10.0
          brace.color = activPal.text
        }
        clefButtons.push(clefButt)
      }
    }
  }
}
