/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import "../"


Drawer {
  property alias selectedClef: clefMenu.selClef
  property alias clefMenu: clefMenu

  width: NOO.factor() * 19; height: parent.height
  visible: true
  background: GlowRect { color: activPal.window; cornerRadius: NOO.factor() / 2; radius: 0 }

  ClefMenu {
    id: clefMenu
    onClicked: { close(); score.clef = cl }
  }

  Component.onCompleted: selectedClef = score.clef
}
