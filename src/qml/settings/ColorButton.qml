/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0


Item {
  id: colorButton
  property color color: "white"
  property real offset: Screen.pixelDensity / 2

  implicitWidth: nootkaWindow.fontSize * 4
  implicitHeight: nootkaWindow.fontSize * 2

  Rectangle {
    id: colorRect
    anchors.fill: parent
    radius: height / 2
    color: enabled ? colorButton.color : disdPal.text
    visible: false
  }

   DropShadow {
    id: shadow
    anchors.fill: colorRect
    horizontalOffset: offset
    verticalOffset: offset
    radius: Screen.pixelDensity
    samples: 17
    color: activPal.shadow
    source: colorRect
  }

  MouseArea {
    anchors.fill: parent
    onPressed: offset = 0
    onReleased: offset = Screen.pixelDensity / 2
    onClicked: colorDialog.open()
  }

  ColorDialog {
    id: colorDialog
    color: colorButton.color
    onAccepted: colorButton.color = colorDialog.color
    modality: Qt.WindowModal
    showAlphaChannel: false
  }
}
