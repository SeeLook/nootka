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

  property int currentMelody: -1

  melodyModel: ListModel {
    id: melMod
  }

  Row {
    Text {
      visible: !melView.visible
      width: melListView.width - Noo.fontSize() * 4
      text: "\n\n" + qsTr("Add here melodies from Music XML files.\nConsider to divide long pieces on parts in external software first.")
      horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
    }
    ListView {
      id: melView
      visible: count > 0
      clip: true; spacing: 1
      width: melListView.width - Noo.fontSize() * 4; height: melListView.height
      add: Transition {
        enabled: GLOB.useAnimations
        NumberAnimation { property: "x"; from: -melListView.width; to: 5 }
      }
      remove: Transition {
        enabled: GLOB.useAnimations
        NumberAnimation { property: "x"; to: -melListView.width }
      }
      populate: Transition {
        enabled: GLOB.useAnimations
        NumberAnimation { property: "x"; from: -melListView.width; to: 5 }
      }
      move: Transition {
        enabled: GLOB.useAnimations
        NumberAnimation { property: "y"; to: -Noo.fontSize() * 4 }
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
  onAppendMelody: melMod.append({})
  onInsertMelody: melMod.insert(melId, {})

  onMelodiesChanged: creator.melodyListChanged()

  function removeWrapper(id) {
    melMod.remove(id)
    removeMelody(id)
    currentMelody = -1
  }
}
