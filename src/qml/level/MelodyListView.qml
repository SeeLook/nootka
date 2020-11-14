/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  implicitHeight: viewRow.height

  property int currentMelody: -1
  property alias viewRoot: viewItem

  melodyModel: ListModel {
    id: melMod
  }

  Row {
    id: viewRow
    Text {
      visible: !melView.visible
      width: melListView.width - Noo.fontSize() * 4
      text: "\n" + qsTr("Add here melodies from Music XML files.\nBut better keep them short, so divide them first in some external software.")
      horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
    }
    Item {
      id: viewItem
      visible: melView.count > 0
      width: melListView.width - Noo.fontSize() * 4; height: Math.max(buttCol.height, melPage.height) //melListView.height
      ListView {
        id: melView
        clip: true; spacing: 1
        anchors { fill: parent; margins: 5 }
        add: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "x"; from: -melListView.width; to: 0 }
        }
        remove: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "x"; to: -melListView.width }
        }
        populate: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "x"; from: -melListView.width; to: 0 }
        }
        move: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "y" }
        }
        moveDisplaced: Transition {
          enabled: GLOB.useAnimations
          NumberAnimation { property: "y" }
        }
        model: melMod
        delegate: MelodyWrapper {
          nr: index
          width: melView.width - 10
          Component.onCompleted: {
            updateMelody()
          }
        }
      }
    }
    Column {
      id: buttCol
      spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.7 : 1)
      leftPadding: Noo.fontSize() / 2; topPadding: Noo.fontSize() / (Noo.isAndroid() ? 2 : 1)
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2.5; bold: true; family: "Sans" }
        text: "+"; textColor: "green"
        onClicked: {
          loadMelody()
          melView.positionViewAtEnd()
        }
      }
      Item { width: 1; height: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1.5) }
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2.5; family: "Nootka" }
        text: "\u2191"; textColor: enabled ? "#008080" : disdPal.text
        enabled: currentMelody > 0
        onClicked: {
          moveMelody(currentMelody, currentMelody - 1)
          currentMelody--
        }
      }
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2.5; family: "Nootka" }
        text: "\u2193"; textColor: enabled ? "#008080" : disdPal.text
        enabled: currentMelody > -1 && currentMelody < melMod.count - 1
        onClicked: {
          moveMelody(currentMelody, currentMelody + 1)
          currentMelody++
        }
      }
      Item { width: 1; height: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1.5) }
      TcuteButton {
        width: Noo.fontSize() * 3
        font { pixelSize: Noo.fontSize() * 2.5; bold: true; family: "Sans" }
        text: "-"; textColor: enabled ? "red" : disdPal.text
        enabled: currentMelody > -1
        onClicked: removeWrapper(currentMelody)
      }
    }
  }
  onAppendMelody: melMod.append({})
  onInsertMelody: melMod.insert(melId, {}) // TODO not implemented yet

  onMelodiesChanged: creator.melodyListChanged()

  function removeWrapper(id) {
    melMod.remove(id)
    removeMelody(id)
    currentMelody = -1
  }
  function moveMelody(from, to) {
    melMod.move(from, to, 1)
    swapMelodies(from, to)
  }
}
