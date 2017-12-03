/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../level"
import "../"


Column {
  id: startExam

  width: parent.width; height: parent.height
  spacing: Noo.fontSize()
  topPadding: Noo.fontSize() / 4

  Row {
    spacing: Noo.fontSize() * 4
    width: parent.width * 0.98
    anchors.horizontalCenter: parent.horizontalCenter
    TcuteButton {
      padding: Noo.fontSize() / 4
      contentItem: Image {
        source: Noo.pix("help")
        sourceSize.height: startExam.height / 16
      }
    }
    Row {
      spacing: Noo.fontSize()
      anchors.verticalCenter: parent.verticalCenter
      Text { text: qsTr("student name:"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
      TextField {
        anchors.verticalCenter: parent.verticalCenter
        placeholderText: qsTr("Enter your name or nick-name.")
        font.pixelSize: Noo.fontSize(); maximumLength: 40
        width: Noo.fontSize() * 30
        horizontalAlignment: TextInput.AlighHCenter
      }
    }
  }

  LevelsSelector {
    id: selector
    width: parent.width; height: parent.height * 0.5
  }

  Component.onCompleted: {
    if (Noo.isAndroid()) {
        dialLoader.buttons = []
    } else {
        dialLoader.standardButtons = 0
        dialLoader.title = qsTr("Start exercises or an exam")
    }
  }
}
