/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import Nootka.Main 1.0


GotIt {
  gotItType: TgotIt.GotExamFlow
  onClicked: close()

  // private
  property real bigImg: Noo.factor() * 2.6
  property var leftTexts: [ "Exercises", "Exams", "Settings" ]
  property var leftImages: [ "practice", "exam", "exam-settings" ]

  Tflickable {
    anchors.fill: parent
    contentHeight: helpCol.height + Noo.factor() * 10
    Column {
      id: helpCol
      width: parent.width
      spacing: Noo.factor()

      Item { width: 1; height: Noo.factor() }
      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: Noo.pix("help"); height: bigImg; width: height * (sourceSize.width / sourceSize.height)
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text
        text: "<h2>" + qsTranslate("TexamHelp", "How does an exercise or an exam work?") + "</h2>"
      }
      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: bigImg * 2
        Image { source: Noo.pix("practice"); height: bigImg; width: height * (sourceSize.width / sourceSize.height) }
        Image { source: Noo.pix("exam"); height: bigImg; width: height * (sourceSize.width / sourceSize.height) }
      }
      LinkText {
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        leftPadding: Noo.factor()
        width: parent.width - Noo.factor()
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        text: gotIt.examHelpText()
      }
      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: Noo.pix("scr"); width: parent.width * 0.8; height: width * (sourceSize.height / sourceSize.width)
      }
      LinkText {
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        leftPadding: Noo.factor()
        width: parent.width - Noo.factor()
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        text: gotIt.examHelpText2()
      }
      Repeater {
        model: [ gotIt.practiceText(), gotIt.examText(), gotIt.optionsText() ]
        Column {
          spacing: Noo.factor()
          Rectangle { width: helpCol.width; height: 1; color: activPal.text }
          Row {
            width: helpCol.width * 0.9
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: bigImg * 2
            Column {
              anchors.verticalCenter: parent.verticalCenter
              spacing: Noo.factor()
              Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: Noo.pix(leftImages[index]); height: bigImg * 2; width: height * (sourceSize.width / sourceSize.height)
              }
              Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: activPal.text
                text: qsTranslate("TexamHelp", leftTexts[index])
              }
            }
            LinkText {
              horizontalAlignment: Text.AlignHCenter
              leftPadding: Noo.factor()
              width: helpCol.width * 0.9 - bigImg * 6
              textFormat: Text.RichText
              wrapMode: Text.WordWrap
              text: modelData
            }
          }
        }
      }
      Rectangle { width: helpCol.width; height: 1; color: activPal.text }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; font { pixelSize: Noo.factor() * 2; bold: true }
        text: qsTranslate("TexamHelp", "GOOD LUCK!")
      }
    }
  }
}
