/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Dialogs 1.2

import "../"


TcuteButton {
  id: colorButton

  property alias title: colorDialog.title

  Behavior on color { enabled: GLOB.useAnimations; ColorAnimation { duration: 750 }}

  implicitWidth: Noo.fontSize() * 3.5
  implicitHeight: Noo.fontSize() * 2

  onClicked: colorDialog.open()

  contentItem: null

  checked: !enabled

  Rectangle { // disable cover
    anchors.fill: background
    visible: !colorButton.enabled
    color: disdPal.text
    radius: Noo.fontSize() / 3
  }

  ColorDialog {
    id: colorDialog
    color: colorButton.color
    onAccepted: colorButton.color = colorDialog.color
    modality: Qt.WindowModal
    showAlphaChannel: color.a < 1.0
  }
}
