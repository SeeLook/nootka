/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import "../"


Tflickable {
  height: parent.height

  property alias color: nooLab.bgColor
  property alias siteAddr: siteAddr

  contentHeight: aboutCol.height
  contentWidth: width

  Column {
    id: aboutCol
    width: parent.width
    spacing: NOO.factor()
    TipRect {
      height: NOO.factor() * (NOO.isAndroid() ? 5 : 7); width: parent.width
      color: nooLab.bgColor; radius: 0
      NootkaLabel {
        id: nooLab
        height: parent.height
        active: false
        anchors.centerIn: parent
        bgColor: NOO.randomColor()
      }
      MouseArea {
        anchors.fill: parent
        onClicked: nooLab.bgColor = NOO.randomColor()
        hoverEnabled: true
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
      }
    }
    LinkText {
      id: siteAddr
      anchors.horizontalCenter: parent. horizontalCenter
      text: ("<a style=\"text-decoration: none; color: %1\" href=\"https://nootka.sourceforge.io\">https://nootka.sourceforge.io</a>").arg(nooLab.bgColor)
      textFormat: Text.RichText
      font { pixelSize: NOO.factor() * 1.5 }
    }
    Tile {
      bgColor: Qt.tint(nooLab.bgColor, NOO.alpha(activPal.base, 230))
      bgBorder { width: 2; color: nooLab.bgColor }
      width: parent.width - NOO.factor()
      Column {
        width: parent.width
        spacing: NOO.factor()
        Text {
          width: parent.width
          font { pixelSize: NOO.factor() * 2; bold: true }
          horizontalAlignment: Text.AlignHCenter
          text: "Nootka " + NOO.version()
          color: activPal.text
        }
        LinkText {
          text: qsTr("Welcome on the board.<br>Nootka is an open source application to help you in learning (and in teaching) classical score notation.")
          + "<br><br>"
          + qsTr("See a <a href=\"%1\">program site</a> for more details and further releases.<br>Any bugs, suggestions, translations and so on, please report to: %2.<br><br>with respects<br>Author")
              .arg("https://nootka.sourceforge.io")
              .arg("<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>")
          width: parent.width * 0.96
          anchors.horizontalCenter: parent.horizontalCenter
          font { pixelSize: NOO.factor() * 1.1 }
          horizontalAlignment: Text.AlignLeft
          wrapMode: Text.WordWrap
        }
      }
      property string betaText: qsTr("This is a beta version and may contain bugs or behave in unexpected ways. Also, it has unfinished features.<br>In spite of that, you are welcome to try it!")
//       description: betaText // uncomment if beta
      descriptionColor: "red"
    }
  }
}
