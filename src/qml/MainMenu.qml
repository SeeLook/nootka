/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0


Item {
  property alias toolBar: toolBar
  property alias scoreMenu: scoreMenu

  TtoolBar { id: toolBar }

  Menu {
    id: scoreMenu
    width: nootkaWindow.fontSize * 20
    x: toolBar.scoreAct.x
    y: score.y

    MenuItem {
      contentItem: MenuButton {
        action: score.openXmlAct
        onClicked: { scoreMenu.close() }
      }
    }

    MenuItem {
      contentItem: MenuButton {
        action: score.extraAccidsAct
        onClicked: { scoreMenu.close() }
      }
    }
    MenuItem {
      contentItem: MenuButton {
        action: score.showNamesAct
        onClicked: { scoreMenu.close() }
      }
    }
    MenuItem {
      contentItem: MenuButton {
        action: score.zoomInAct
        onClicked: { scoreMenu.close() }
      }
    }
    MenuItem {
      contentItem: MenuButton {
        action: score.zoomOutAct
        onClicked: { scoreMenu.close() }
      }
    }
  }
}
