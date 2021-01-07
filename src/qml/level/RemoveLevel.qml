/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TpopupDialog {
  id: rmLevel

  property string levelName
  property alias levelFile: fileText.text

  signal remove(var fromDisk)

  bgColor: Qt.tint(activPal.window, Noo.alpha("red", 20))
  border { color: "red"; width: Noo.fontSize() / 4.0 }
  visible: true; modal: true
  width: innerCol.width * 1.2; height: innerCol.height + Noo.fontSize() * 5

  acceptButton.text: Noo.TR("QFileDialog", "Remove")
  acceptButton.pixmap: Noo.pix("delete")

  Column {
    id: innerCol
    spacing: Noo.fontSize()
    anchors.horizontalCenter: parent.horizontalCenter
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
      font.pixelSize: Noo.fontSize() * 1.5
      text: qsTr("Remove level %1 from the list").arg("<b>" + levelName + "</b>")// + "<br>"
    }
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


