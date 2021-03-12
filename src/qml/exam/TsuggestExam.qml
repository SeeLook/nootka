/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


TpopupDialog {
  property alias entireVisible: entireRadio.visible

  caption: qsTr("Start an exam")

  bgColor: Qt.tint(activPal.window, NOO.alpha(GLOB.correctColor, 20))
  border { color: GLOB.correctColor; width: NOO.factor() / 4.0 }
  visible: true; modal: true
  width: outerCol.width * 1.2; height: outerCol.height + NOO.factor() * 7
  glowRect.radius: NOO.factor()

  acceptButton.text: NOO.TR("QMessageBox", "OK")
  rejectButton.visible: false

  ButtonGroup { id: gr; buttons: innerCol.children }

  Column {
    id: outerCol
    spacing: NOO.factor()
    anchors.horizontalCenter: parent.horizontalCenter

    Text {
      color: activPal.text
      font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2); bold: true }
      text: qsTr("You are very good in this exercise!<br>Would you like to pass an exam on the same level and got a certificate?")
      horizontalAlignment: Text.AlignHCenter
    }

    Column {
      id: innerCol
      spacing: NOO.factor() / 2
      anchors.horizontalCenter: parent.horizontalCenter
      TradioButton { text: qsTr("Sure! Lets start an exam!"); checked: true }
      TradioButton { text: qsTr("Not now, ask me for a moment.") }
      TradioButton { id: entireRadio; text: qsTr("No thanks, but check entire melody from now.") }
      TradioButton { text: qsTr("Not in this exercise.") }
      TradioButton { text: qsTr("Never more suggest me to start an exam!") }
    }
  }

  onAccepted: {
    for (var b = 0; b < gr.buttons.length; ++b) {
      if (gr.buttons[b].checked) {
        executor.suggestDialogClosed(b)
        break
      }
    }
  }

  onRejected: executor.suggestDialogClosed(-1)

  onClosed: destroy()
}
