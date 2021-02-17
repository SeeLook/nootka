/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Main 1.0
import "../"
import "../sound"


GotIt {
  gotItType: TgotIt.GotSoundInfo
  visible: true

  onClicked: close()

  Tflickable {
    height: parent.height
    contentHeight: sCol.height + NOO.factor() * 10
    contentWidth: width

      Column {
        id: sCol
        width: parent.width - NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor()
        Item { width: NOO.factor() * 10; height: NOO.factor(); visible: !NOO.isAndroid() }
        TipRect {
          width: sCol.width - NOO.factor(); height: headRow.height + NOO.factor()
          anchors.horizontalCenter: parent.horizontalCenter
          color: Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 100))
          Row {
            id: headRow
            spacing: NOO.factor() * 2
            anchors.centerIn: parent
            Text {
              font { family: "Nootka"; pixelSize: NOO.factor() * 5 }
              color: activPal.highlight
              text: "r"; rotation: 180
            }
            LinkText {
              anchors.verticalCenter: parent.verticalCenter
              font { bold: true; pixelSize: NOO.factor() * 2 }
              text: qsTr("Nootka can hear You!")
            }
            Text {
              font { family: "Nootka"; pixelSize: NOO.factor() * 5 }
              color: activPal.highlight
              text: "r"
            }
          }
        }
        Text {
          width: parent.width * 0.96; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText
          lineHeight: NOO.isAndroid() ? 1 : 1.5
          text: "<br>"
            + qsTr("Nootka recognizes played sounds and theirs duration (rhythms). But to achieve accurate results it requires some preparations and a little patience. Here are some clues:")
        }
        Repeater {
          model: [ qsTr("Be sure your system is able to record what are you playing."),
                  qsTr("Adjust minimal note volume a little below of your input volume level.") ]
          Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor()
            Rectangle {
              anchors.verticalCenter: parent.verticalCenter
              width: NOO.factor() * 0.7; height: width; radius: width / 2
              color: activPal.text
            }
            Text {
              width: sCol.width * 0.96 - NOO.factor() * 2; wrapMode: Text.WordWrap
              color: activPal.text; textFormat: Text.StyledText
              text: modelData
            }
          }
        }
        Text {
          width: parent.width * 0.8; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText
          horizontalAlignment: Text.AlignHCenter
          text: qsTr("Play a few sounds to figure out their maximal volume,<br>then set level knob about 10-20% below.")
        }
        Row {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: NOO.factor()
          TipRect {
            color: activPal.base
            width: NOO.factor() * 4; height: NOO.factor() * 2.5
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              y: height * -0.25
              color: activPal.text
              font { family: "Scorek"; pixelSize: NOO.factor() * 2 }
              text: gotIt.noteName
            }
          }
          TipRect {
            color: activPal.window
            width: Math.min(NOO.factor() * 42, sCol.width * 0.98 - NOO.factor() * 10); height: NOO.factor() * 2.5
            VolumeBar {
              id: volBar
              anchors.centerIn: parent
              width: Math.min(NOO.factor() * 40, parent.width * 0.9); height: NOO.factor() * 2
              knobVisible: true
              Timer {
                repeat: true; interval: 75; running: true
                onTriggered: volBar.volume = SOUND.inputVol()
              }
            }
          }
          TipRect {
            color: activPal.base
            width: NOO.factor() * 4; height: NOO.factor() * 2.5
            Text {
              anchors.centerIn: parent
              color: activPal.text
              font.bold: true
              text: gotIt.maxVolume + "%"
            }
          }
        }
        Text {
          visible: NOO.isAndroid()
          width: parent.width * 0.8; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText
          horizontalAlignment: Text.AlignHCenter
          text: qsTr("Later, you can adjust the volume level in Nootka tuner window.<br>Invoke it from main menu or use any volume key.")
        }
        Item { width: NOO.factor() * 10; height: NOO.factor() }
        Repeater {
          model: [ qsTr("Aim to play loud and clear and avoid dirty sounds specific to your instrument."),
                   qsTr("If you want Nootka to write your melody with rhythm then you have to play exactly in selected tempo - preferably with a metronome. But during practice, when you are playing melody given by the application, the tempo can be at will unless some level defines it.") ]
          Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor()
            Rectangle {
              anchors.verticalCenter: parent.verticalCenter
              width: NOO.factor() * 0.7; height: width; radius: width / 2
              color: activPal.text
            }
            Text {
              width: sCol.width * 0.96 - NOO.factor() * 2; wrapMode: Text.WordWrap
              color: activPal.text; textFormat: Text.StyledText
              text: modelData
            }
          }
        }
        Text {
          width: parent.width * 0.96; wrapMode: Text.WordWrap
          anchors.horizontalCenter: parent.horizontalCenter
          color: "red"; horizontalAlignment: Text.AlignHCenter
          text: "<br><br>Pitch detection is very important feature of this app but it may be confusing for users.<br>"
          + "Help to make this text clear as mach as possible.<br>"
        }
      }
  }
}
