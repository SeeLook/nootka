/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

import Nootka 1.0


Drawer {
  width: Noo.fontSize() * 20
  height: dialLoader.height
  ListView {
    spacing: Noo.fontSize() / 4
    anchors.fill: parent
    model: dialLoader.buttons
    delegate: Component {
      MenuButton {
        property int role: dialLoader.buttons[index]
        action: Taction {
          text: dialogObj.stdButtonText(dialLoader.buttons[index])
          icon: dialogObj.stdButtonIcon(dialLoader.buttons[index])
        }
        onClicked: mapRole(role)
      }
    }
  }

  function mapRole(role) {
    switch (role) {
      case StandardButton.Ok: dialLoader.accept(); break
      case StandardButton.Apply: dialLoader.apply(); break
      case StandardButton.Cancel: dialLoader.reject(); break
      case StandardButton.RestoreDefaults: dialLoader.reset(); break
      case StandardButton.Help: dialLoader.help(); break
      case StandardButton.Close: dialLoader.close(); break
    }
    close()
  }
}
