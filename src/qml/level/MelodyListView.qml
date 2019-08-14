/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import Nootka 1.0
import Score 1.0
import "../score"
import "../"


TmelodyListView {
  id: melListView

  property int currentMelody: -1

  // private
  property var wrappers: []

  Row {
    Text {
      visible: !melView.visible
      width: melListView.width - Noo.fontSize() * 4
      text: "\n\n" + qsTr("Add here melodies from Music XML files.\nConsider to divide long pieces on parts in external software first.")
      horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
    }
    Tflickable {
      id: melView
      visible: false
      width: melListView.width - Noo.fontSize() * 4; height: melListView.height
      contentHeight: melColumn.height
      Column {
        id: melColumn
  //       clip: true
        width: parent.width
        spacing: 1; padding: 5
        add: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "x"; from: -melListView.width; to: 5 }
        }
        move: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "y" }
        }
      }
    }
    Column {
      spacing: Noo.fontSize() * 2
      leftPadding: Noo.fontSize() / 2; topPadding: Noo.fontSize()
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2; bold: true; family: "Sans" }
        text: "+"; textColor: "green"
        onClicked: loadMelody()
      }
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2; bold: true; family: "Sans" }
        text: "-"; textColor: "red"
        enabled: currentMelody > -1
        onClicked: removeWrapper(currentMelody)
      }
    }
  }
  onAddScore: {
    var c = Qt.createComponent("qrc:/level/MelodyWrapper.qml")
    var s = c.createObject(melColumn, { "nr": melodiesCount - 1, "title": title(melodiesCount - 1), "composer": composer(melodiesCount - 1) })
    s.width = Qt.binding(function() { return melColumn.width - 10 })
    wrappers.push(s)
    setScore(s.nr, s.scoreObj)
    melView.visible = true
  }

  onMelodiesChanged: creator.melodyListChanged()
  onRemoveScore: removeWrapper(id)

  function removeWrapper(id) {
    if (id < wrappers.length) {
      removeMelody(id)
      wrappers[id].destroy()
      wrappers.splice(id, 1)
      currentMelody = -1
      for (var i = 0; i < wrappers.length; ++i)
        wrappers[i].nr = i
    }
    if (wrappers.length < 1)
      melView.visible = false
  }
}
