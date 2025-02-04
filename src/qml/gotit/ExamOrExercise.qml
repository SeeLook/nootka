/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Nootka 1.0
import "../"

GotIt {
  onClicked: close()

  Tflickable {
    anchors.fill: parent
    contentHeight: helpCol.height + NOO.factor() * 10
    Column {
      id: helpCol
      width: parent.width
      spacing: NOO.factor()

      Row {
        spacing: NOO.factor() * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
          source: NOO.pix("practice")
          height: NOO.factor() * 4; width: height * (sourceSize.width / sourceSize.height)
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: activPal.text
          text: qsTranslate("TstartExamDlg", "To exercise or to pass an exam?")
          font { bold: true; pixelSize: NOO.factor() * 1.5 }
        }
        Image {
          source: NOO.pix("exam")
          height: NOO.factor() * 4; width: height * (sourceSize.width / sourceSize.height)
        }
      }

      LinkText {
        id: exerOrExamText
        anchors.horizontalCenter: parent.horizontalCenter
        leftPadding: NOO.factor()
        width: parent.width - NOO.factor()
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        text: gotIt.exerOrExamHelpTxt()
      }
    }
  }
}
