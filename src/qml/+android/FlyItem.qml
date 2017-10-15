/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0


TcuteButton {
  property Taction action: null

  radius: width / 2

  Image {
    source: action ? action.icon : ""
    sourceSize.width: parent.width * 0.6
    x: (parent.width - width) / 2; y: (parent.height - height) / 2
  }
} 
