/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Tmenu {
  id: menu
  x: toolBar.scoreAct.x
  y: score.y

  Repeater {
    model: score.scoreActions
    MenuItem {
      padding: 0
      width: menu.width
      contentItem: MenuButton {
        action: modelData
        onClicked: close()
        Rectangle { width: parent.width; height: 1; color: activPal.text; y: parent.height - 1 }
      }
    }
  }
  MenuItem {
    padding: 0
    width: menu.width
    contentItem: MenuButton {
      action: Taction {
        text: qsTr("notes", "musical notes of course") + "   ⋮"; icon: "help"
        onTriggered: {
          notesLoader.active = true;
          notesLoader.item.open()
          close()
        }
      }
    }
  }

  property var rGlyphs: [ "C", "D", "E", "F", "G", "\ue109", "." ]
  Loader { id: notesLoader; sourceComponent: notesComp; active: false }
  Component {
    id: notesComp
    Tmenu {
      id: noteMenu
      x: menu.width / 4; y: menu.height * -0.9
      Repeater {
        model: score.noteActions
        MenuItem {
          padding: 0
          width: menu.width
          contentItem: MenuButton {
            action: modelData
            onClicked: close()
            Rectangle { width: parent.width; height: index === score.noteActions.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
            Text {
              text: rGlyphs[index]
              color: activPal.text
              x: (Noo.fontSize() * 3.2 - width) / 2; anchors.verticalCenter: parent.verticalCenter
              font { pixelSize: Noo.fontSize() * 1.5; family: "nootka"; }
            }
          }
        }
      }
    }
  }
}
