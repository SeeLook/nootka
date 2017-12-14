/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  contentHeight: examCol.height
  contentWidth: width

  Column {
    id:examCol

    padding: Noo.fontSize() * 3
    width: parent.width
    Text { text: "Work in progress......."; anchors.horizontalCenter: parent.horizontalCenter }
  }
}
