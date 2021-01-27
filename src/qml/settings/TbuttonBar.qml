/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ListView {
  id: buttBar
  orientation: ListView.Horizontal
  spacing: Noo.fontSize()
  width: parent.width - Noo.fontSize() / 2
  currentIndex: 0
  leftMargin: Noo.isAndroid() ? 0 : Noo.fontSize() * 2
  clip: true

  delegate: TabButton {
    id: tb
    onClicked:  buttBar.currentIndex = index
    checked: buttBar.currentIndex === index
    contentItem: Text {
      anchors.centerIn: parent
      text: (index + 1) + ". " + modelData
      color: checked ? activPal.highlightedText : activPal.buttonText
      style: tb.activeFocus ? Text.Sunken : Text.Normal
      styleColor: activPal.text
    }
    Component.onCompleted: { // only way to get access to TabButton components
      background.color = Qt.binding(function() { return checked ? activPal.highlight : activPal.button })
      buttBar.height = height
    }
  }
}
