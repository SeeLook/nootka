/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Menu {
  id: menu
  width: Noo.fontSize() * 20
  x: toolBar.scoreAct.x
  y: score.y

  background: TipRect { color: activPal.window; shadowRadius: Noo.fontSize() }

  Repeater {
    model: score.scoreActions
    MenuItem {
      width: menu.width
      contentItem: MenuButton {
        action: modelData
        onClicked: close()
      }
    }
  }
}
