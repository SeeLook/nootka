/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


CheckBox {
  id: chB
  property alias textColor: content.color

  contentItem: Text {
    id: content
    text: chB.text
    anchors.verticalCenter: parent.verticalCenter
    leftPadding: indicator.width + font.pixelSize / 2
    color: chB.enabled ? activPal.text : disdPal.text
    font: chB.font
  }
}
