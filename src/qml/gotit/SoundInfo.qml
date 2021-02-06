/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Main 1.0
import "../"
import "../sound"


GotIt {
  gotItType: TgotIt.GotSoundInfo
  againVisible: false
  visible: true

  onClicked: destroy()

  Tflickable {
    height: parent.height
    contentHeight: sCol.height + Noo.factor() * 10
    contentWidth: width

      Column {
        id: sCol
        width: parent.width - Noo.factor() * 2
        spacing: Noo.factor()
        Row {
          spacing: Noo.factor() * 2
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            font { family: "Nootka"; pixelSize: Noo.factor() * 5 }
            color: activPal.highlight
            text: "r"; rotation: 180
          }
          LinkText {
            anchors.verticalCenter: parent.verticalCenter
            font { bold: true; pixelSize: Noo.factor() * 2 }
            text: qsTr("Nootka can hear You!")
          }
          Text {
            font { family: "Nootka"; pixelSize: Noo.factor() * 5 }
            color: activPal.highlight
            text: "r"
          }
        }
        Text {
          width: parent.width * 0.96; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText
          lineHeight: 1.5
          text: qsTr("Nootka recognizes played sounds and theirs duration (rhythms). But to achieve accurate results it requires some preparations and a little patience. Here are some clues:") + "<ul>"
          + "<li>" + qsTr("Be sure your system is able to record what are you playing.") + "</li>"
          + "<li>" + qsTr("Adjust minimal note volume a little below of your input volume level.") + "</li></ul>"
        }
        TipRect {
          color: activPal.window
          anchors.horizontalCenter: parent.horizontalCenter
          width: Math.min(Noo.factor() * 42, parent.width * 0.92); height: Noo.factor() * 2.5
          VolumeBar {
            id: volBar
            anchors.centerIn: parent
            width: Math.min(Noo.factor() * 40, parent.width * 0.9); height: Noo.factor() * 2
            Timer {
              repeat: true; interval: 75; running: true
              onTriggered: volBar.volume = SOUND.inputVol()
            }
          }
        }
        Text {
          width: parent.width * 0.96; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText
          lineHeight: 1.5
          text: "<br><ul><li>" + qsTr("Aim to play loud and clear and avoid dirty sounds specific to your instrument.") + "</li>"
          + "<li>" + qsTr("Rhythms detection during exercises and exams is more 'humane' - Nootka knows what rhythm to expect. In contrary to playing to 'empty' score when high rhythmic precision is required, so better use metronome then.") + "</li>"
          + "</ul><br>"
        }
        Text {
          width: parent.width * 0.96; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: "red"; horizontalAlignment: Text.AlignHCenter
          text: "Pitch detection is very important feature of this app but it may be confusing for users.<br>"
          + "Help to make this text clear as mach as possible.<br>"
        }
      }
  }
}
