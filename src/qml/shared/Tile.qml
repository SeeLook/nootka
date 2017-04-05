/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0


Frame {
  property alias description: descText.text
  property alias descriptionColor: descText.color
  default property alias content: container.data

  background: Rectangle {
    id: bg
    color: enabled ? Qt.lighter(activPal.window, 1.05) : Qt.darker(disdPal.window, 1.5)
    border.color: disdPal.text
    radius: nootkaWindow.fontSize / 4
  }

  width: parent.width * 0.96
  height: (descText.text === "" ? 0 : descText.height) + container.height + nootkaWindow.fontSize * 2

  Column {
      id: col
      spacing: nootkaWindow.fontSize / 2
      width: parent.width

      Item {
        id: container
        width: parent.width - nootkaWindow.fontSize
        height: childrenRect.height
      }

      Text {
        id: descText
        anchors.bottom: parent.Bottom
        width: parent.width
        font.pixelSize: nootkaWindow.fontSize * 0.8
        textFormat: Text.RichText
        horizontalAlignment: Text.AlignHCenter
        color: enabled ? activPal.text : disdPal.text
        wrapMode: Text.WordWrap
      }

  }
}
