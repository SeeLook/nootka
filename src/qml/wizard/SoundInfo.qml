/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import "../sound"


Tflickable {
  height: parent.height
  contentHeight: sTile.height
  contentWidth: width

  Tile {
    id: sTile
    bgColor: Qt.tint(nootkaWindow.labelColor, Noo.alpha(activPal.base, 230))
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    anchors.verticalCenter: parent.verticalCenter
    Column {
      id: sCol
      width: parent.width - Noo.fontSize() * 2
      spacing: Noo.fontSize()
      Row {
        spacing: Noo.fontSize() * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          font { family: "Nootka"; pixelSize: Noo.fontSize() * 5 }
          color: nootkaWindow.labelColor
          text: "r"; rotation: 180
        }
        LinkText {
          anchors.verticalCenter: parent.verticalCenter
          font { bold: true; pixelSize: Noo.fontSize() * 2 }
          text: qsTr("Nootka can hear You!")
        }
        Text {
          font { family: "Nootka"; pixelSize: Noo.fontSize() * 5 }
          color: nootkaWindow.labelColor
          text: "r"
        }
      }
      Text {
        width: parent.width * 0.96; wrapMode: Text.WordWrap
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; textFormat: Text.RichText
        text: qsTr("Nootka recognizes played sounds and theirs duration (rhythms). But to achieve accurate results it requires some preparations and a little patience. Here are some clues:") + "<ul>"
        + "<li>" + qsTr("Be sure your system is able to record what are you playing.") + "</li>"
        + "<li>" + qsTr("Adjust minimal note volume a little below of your input volume level.") + "</li></ul>"
      }
      VolumeBar {
        id: volBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.9; height: Noo.fontSize() * 2
        Timer {
          repeat: true; interval: 100; running: true
          onTriggered: volBar.volume = Math.random() * 0.9
        }
      }
      Text {
        width: parent.width * 0.96; wrapMode: Text.WordWrap
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; textFormat: Text.RichText
        text: "<ul><li>" + qsTr("Aim to play loud and clear and avoid dirty sounds specific to your instrument.") + "</li>"
        + "<li>" + qsTr("Rhythms detection during exercises and exams is more 'humane' - Nootka knows what rhythm to expect. In contrary to playing to 'empty' score when high rhythmic precision is required, so better use metronome then.") + "</li>"
        + "</ul><br>"
      }
    }
    description: "<font color=\"red\">Pitch detection is very important feature of this app but it may be confusing for users.<br>"
    + "Help to make this text clear as mach as possible.</font><br>"
  }
}
