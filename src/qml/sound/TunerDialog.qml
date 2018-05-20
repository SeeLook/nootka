/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"


TtunerDialogItem {
  id: tunerDialog
  width: parent.width; height: parent.height

  IntonationBar {
    id: intoBar
    width: parent.width * 0.86; height: Noo.fontSize() * 2
    anchors.horizontalCenter: parent.horizontalCenter
    deviation: tunerDialog.deviation
  }

  Row {
    anchors { horizontalCenter: parent.horizontalCenter; top: intoBar.bottom }
    spacing: Noo.fontSize()
    Rectangle {
      width: Noo.fontSize() * 4; height: Noo.fontSize() * 3; radius: height / 8; color: activPal.base
      Text {
        y: height * -0.24; x: (parent.width - width) / 2
        font { family: "Scorek"; pixelSize: Noo.fontSize() * 2 }
        text: noteName
        color: activPal.text; textFormat: Text.StyledText
      }
    }
    Rectangle {
      width: Noo.fontSize() * 12; height: Noo.fontSize() * 3; radius: height / 8; color: activPal.base
      Text {
        anchors.centerIn: parent
        font { pixelSize: Noo.fontSize() * 2; bold: true }
        text: frequency + " Hz"
        color: activPal.text
      }
    }
  }

  Tile {
    anchors.bottom: parent.bottom
    description: qsTranslate("SoundPage", "Frequency of detected note. You can use this for tuning.")
    Flow {
      width: parent.width * 0.9
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize()
      Repeater {
        model: tuningModel
        Text {
          font { pixelSize: Noo.fontSize() * 1.5 }
          color: activPal.text; textFormat: Text.RichText
          text: modelData
        }
      }
    }
  }

  Component.onCompleted: {
    if (Noo.isAndroid()) {
        dialLoader.buttons = [DialogButtonBox.Close]
    } else {
        dialLoader.standardButtons = DialogButtonBox.Close
        dialLoader.title = qsTr("Nooter - Nootka tuner")
    }
  }

}
