/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Menu {
  id: menu
  width: Noo.fontSize() * 20
  x: toolBar.melodyAct.x
  y: score.y
  scale: GLOB.useAnimations ? 0.1 : 1.0

  background: TipRect { shadowRadius: Noo.fontSize() }

  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0.1 }}

  Repeater {
    model: score.melodyActions
    MenuItem {
      padding: 0
      width: menu.width
      contentItem: MenuButton {
        action: modelData
        onClicked: close()
      }
    }
  }
}
