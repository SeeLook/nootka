/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


TpopupDialog {
  id: warnPopup

  property string fName: ""

  caption: NOO.TR("dummy", "Update Nootka")

  visible: true; modal: true
  width: warnCol.width + NOO.factor() * 2; height: warnCol.height + NOO.factor() * 7

  acceptButton.text: NOO.TR("GotIt", "GOT IT!")
  rejectButton.visible: false

  bgColor: Qt.tint(activPal.base, NOO.alpha("green", 20))
  border { color: "red"; width: NOO.factor() / 4.0 }
  glowRect.radius: NOO.factor() * 2 

  Column {
    id: warnCol
    x: (parent.width - width) / 2; y: (parent.height - height) / 2;
    Text {
      color: activPal.text
      font { pixelSize: NOO.factor() * 1.5; bold: true }
      lineHeight: 1.25
      horizontalAlignment: Text.AlignHCenter
      text: NOO.TR("dummy", "File:<br><font color=\"red\"> %1</font><br>was created with newer Nootka version.<br>To open it you have to update Nootka.").arg(fName)
    }
  }

  onClosed: destroy()
}
