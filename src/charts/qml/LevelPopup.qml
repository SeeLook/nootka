/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import "../level"


Tmenu {
  id: lPop
  property alias levelPreview: levelPreview
  width: Noo.fontSize() * 40; height: parent.height / 2
  x: (parent.width - width) / 2; y: parent.height / 12
  visible: true

  Column {
    width: parent.width; padding: Noo.fontSize() / 2
    LevelPreview {
      id: levelPreview
      width: parent.width - Noo.fontSize(); height: lPop.height - Noo.fontSize()
    }
  }

  onClosed: destroy()
}
