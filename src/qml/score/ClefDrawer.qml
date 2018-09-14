/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Drawer {
  property alias selectedClef: clefMenu.selClef
  width: Noo.fontSize() * 19; height: parent.height
  visible: true
  background: TipRect { color: activPal.window; radius: 0 }

  ClefMenu {
    id: clefMenu
    onClicked: { close(); score.clef = cl }
  }

  Component.onCompleted: selectedClef = score.clef
}
