/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Tmenu {
  id: menu
  x: toolBar.melodyAct.x
  y: score.y

  Repeater {
    model: score.melodyActions
    MenuItem {
      padding: 0
      width: menu.width
      contentItem: MenuButton {
        action: modelData
        onClicked: close()
        Rectangle { width: parent.width; height: index === score.melodyActions.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
      }
    }
  }
}
