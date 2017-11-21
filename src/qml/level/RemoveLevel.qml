/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Dialog {
  id: rmLevel

  property string levelName
  property alias levelFile: fileText.text

  signal remove(var fromDisk)

  visible: true
  x: (parent.width - width) / 2; y: (parent.height - height) / 2
  background: TipRect { color: activPal.window }
  standardButtons: Dialog.Ok | Dialog.Abort

  Column {
    spacing: Noo.fontSize()
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
      font.pixelSize: Noo.fontSize() * 1.5
      text: qsTr("Remove level %1 from the list").arg("<b>" + levelName + "</b>")
    }
    Item { width: parent.width; height: Noo.fontSize() * 2 }
    TcheckBox {
      id: rmFileChB
      anchors.horizontalCenter: parent.horizontalCenter
      text: qsTr("Also delete level file:")
      checked: false
    }
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      id: fileText
      color: activPal.text
      font.bold: true
    }
  }

  onAccepted: rmLevel.remove(rmFileChB.checked)
}


