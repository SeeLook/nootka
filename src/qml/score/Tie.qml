/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Text {
  id: tie
  property alias xScale: sc.xScale
  property bool stemDown: false

  font { family: "Scorek"; pixelSize: 7 }
  text: stemDown ? "\ue204" : "\ue1fd"
  transform: Scale { id: sc }
  y: -0.5
  z: - 20
}
