/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

Rectangle {
  x: score.singleNote ? 0.5 : - 1.0
  height: 0.2
  width: 4.0
  color: noteCursor.color
}
