/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import "../exam"
import Nootka.Main 1.0


GotIt {
  gotItType: TgotIt.GotExamFlow
  onClicked: close()

  // private
  property real bigImg: NOO.factor() * (NOO.isAndroid() ? 1.8 : 3)
  property var leftTexts: [ qsTranslate("TexamHelp", "Exercises"), qsTranslate("TexamHelp", "Exams"), qsTranslate("TexamHelp", "Settings") ]
  property var leftImages: [ "practice", "exam", "exam-settings" ]
  property alias executor: helpCol

  Component {
    id: tipComp
    GlowRect {
      property alias text: glowText.text
      width: glowText.width; height: glowText.height; radius: NOO.factor()
      color: Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50))
      border { width: NOO.factor() / 6; color: activPal.highlight }
      Text {
        id: glowText
        color: activPal.text; padding: NOO.factor()
      }
    }
  }

  Component {
    id: textComp
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
      height: NOO.factor() * 7
      leftPadding: NOO.factor()
      width: helpCol.width - NOO.factor()
      color: activPal.text; wrapMode: Text.WordWrap
    }
  }

  Tflickable {
    anchors.fill: parent
    contentHeight: helpCol.height + NOO.factor() * 10
    Column {
      id: helpCol
      width: parent.width
      spacing: NOO.factor()

      Item { width: 1; height: NOO.factor() }
      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: NOO.pix("help"); height: bigImg; width: height * (sourceSize.width / sourceSize.height)
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text
        text: "<h1>" + qsTranslate("TexamHelp", "How does an exercise or an exam work?") + "</h1>"
      }
      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: bigImg * 2
        Image { source: NOO.pix("practice"); height: bigImg; width: height * (sourceSize.width / sourceSize.height) }
        Image { source: NOO.pix("exam"); height: bigImg; width: height * (sourceSize.width / sourceSize.height) }
      }
      Loader {
        sourceComponent: textComp
        anchors.horizontalCenter: parent.horizontalCenter
        onLoaded: item.text = "<h3>" + qsTranslate("TexamHelp", " Briefly: Nootka give you a question and you give an answer...") + "</h3>"
      }
      Loader {
        sourceComponent: tipComp
        anchors.horizontalCenter: parent.horizontalCenter
        onLoaded: item.text = gotIt.getQuestionText()
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        leftPadding: NOO.factor(); lineHeight: 1.5
        width: parent.width - NOO.factor(); horizontalAlignment: Text.AlignHCenter
        color: activPal.text; wrapMode: Text.WordWrap; textFormat: Text.RichText
        text: ("<br><span style=\"background-color: %1\">").arg(NOO.alpha(GLOB.wrongColor, 40))
              + qsTranslate("TexamHelp", "Questions are marked with this color and \"?\" mark.") + "</span><br>"
              + qsTranslate("TexamHelp", "To give an answer, select it on <span style=\"%1\">Nootka's element with that color.</span><br>")
              .arg("background-color:" + NOO.alpha(GLOB.correctColor, 40))
      }
      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        source: NOO.pix("scr")
        width: Math.min(NOO.factor() * 60, parent.width * 0.8); height: width * (sourceSize.height / sourceSize.width)
      }
      Loader {
        sourceComponent: tipComp
        anchors.horizontalCenter: parent.horizontalCenter
        onLoaded: item.text = gotIt.confirmText()
      }
      Item { width: 1; height: NOO.factor() * 2 }
      Repeater {
        model: [ gotIt.practiceExplained(), gotIt.examExplained(), gotIt.optionsText() ]
        Column {
          spacing: NOO.factor()
          Rectangle { width: helpCol.width; height: 1; color: activPal.text }
          Row {
            width: helpCol.width * 0.9
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: bigImg * (NOO.isAndroid() ? 1 : 2)
            Column {
              anchors.verticalCenter: parent.verticalCenter
              spacing: NOO.factor()
              Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: NOO.pix(leftImages[index]); height: bigImg * 2
                width: height * (sourceSize.width / sourceSize.height)
              }
              Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: activPal.text
                text: qsTranslate("TexamHelp", leftTexts[index])
              }
            }
            Column {
              anchors.verticalCenter: parent.verticalCenter
              spacing: NOO.factor()
              GlowRect {
                visible: index !== 2
                width: glowText2.width; height: glowText2.height; radius: NOO.factor()
                color: Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50))
                border { width: NOO.factor() / 6; color: activPal.highlight }
                Text {
                  id: glowText2
                  width: NOO.factor() * 30; wrapMode: Text.WordWrap
                  color: activPal.text; padding: NOO.factor()
                  text: index === 0 ? gotIt.practiceText() : gotIt.examText()
                }
              }
              Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: activPal.text; wrapMode: Text.WordWrap
                width: helpCol.width * 0.9 - bigImg * 4
                text: modelData
              }
            }
          }
        }
      }
      Rectangle { width: helpCol.width; height: 1; color: activPal.text }
      LinkText {
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - NOO.factor()
        textFormat: Text.RichText; wrapMode: Text.WordWrap
        text: gotIt.onlineDocP("exercises")
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; font { pixelSize: NOO.factor() * 2; bold: true }
        text: qsTranslate("TexamHelp", "GOOD LUCK!")
      }
    }
  }
}
