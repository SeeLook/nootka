/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0


Rectangle {
  id: root
  color: activPal.base
  width: parent.width; height: text.height

  property alias textColor: text.color
  property alias font: text.font
  property string text: ""

  Text {
    id: text
    anchors.centerIn: parent
    text: '<font size="6"><b>' + root.text + '</b></font>'
    textFormat: Text.StyledText
    color: activPal.text
  }
}
