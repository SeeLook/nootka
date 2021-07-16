/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka 1.0


Tmenu {
  id: menu
  x: NOO.isAndroid() ? (nootkaWindow.width - width) / 2 : toolBar.scoreAct.x
  y: NOO.isAndroid() ? (nootkaWindow.height - height) / 2 : score.y

  onAboutToShow: SOUND.stopListen()
  onAboutToHide: SOUND.startListen()

  Repeater {
    model: score.scoreActions
    MenuItem {
      padding: 0
      width: menu.width
      enabled: modelData.enabled
      contentItem: MenuButton {
        action: modelData
        onClicked: close()
        Rectangle { width: parent.width; height: index === score.scoreActions.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
      }
    }
  }

  Connections {
    target: score.notesMenuAct
    onTriggered: {
      notesLoader.active = true;
      notesLoader.item.open()
      close()
    }
  }

  property var rGlyphs: [ "#", "b", "C", "D", "E", "F", "G", "\ue109", ".", "\ue18c" ]
  Loader { id: notesLoader; sourceComponent: notesComp; active: false }
  Component {
    id: notesComp
    Tmenu {
      id: noteMenu
      parent: menu.parent
      x: menu.x +  menu.width / 4; y: score.y + menu.width / 8

      Repeater {
        model: score.noteActions
        MenuItem {
          padding: 0
          width: menu.width
          contentItem: MenuButton {
            id: mb
            action: modelData
            onClicked: close()
            Rectangle { width: parent.width; height: index === score.noteActions.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
            Text {
              text: rGlyphs[index]
              color: activPal.text
              x: (NOO.factor() * 3.2 - width) / 2; anchors.verticalCenter: parent.verticalCenter
              font { pixelSize: NOO.factor() * 1.5; family: "nootka"; }
              scale: GLOB.useAnimations && !mb.containsPress && mb.containsMouse ? 1.4 : 1.0
              Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
            }
          }
        }
      }

    }
  } // Component

  Connections {
    target: score.transposeAct
    onTriggered: {
      var tPop = transComp.createObject(nootkaWindow)
      tPop.initKey = score.keySignature
      close()
    }
  }

  Component {
    id: transComp
    TpopupDialog {
      id: transPopup
      property alias initKey: transpose.initialKey
      visible: true; modal: true
      width: transpose.width + NOO.factor(); height: transpose.height + NOO.factor() * 8
      caption: NOO.TR("Transpose", "Transpose")
      Transpose {
        id: transpose
        headerVisible: false
      }
      onClosed: destroy()
      onAccepted: {
        if (transpose.toKey || transpose.byInterval) {
          if (transpose.toKey)
            score.keySignature = transpose.selectedKey
          var sObj = score.scoreObj
          sObj.transpose(transpose.outShift, transpose.outScaleToRest,
                                   transpose.inInstrumentScale ? GLOB.loNote() : sObj.lowestNote(),
                                   transpose.inInstrumentScale ? GLOB.hiNote() : sObj.highestNote())
        }
      }
    }
  }
}
