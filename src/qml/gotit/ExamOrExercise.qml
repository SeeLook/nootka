/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


GotIt {
  onClicked: close()

  Tflickable {
    anchors.fill: parent
    contentHeight: helpCol.height + Noo.factor() * 10
    Column {
      id: helpCol
      width: parent.width
      Item { width: 1; height: Noo.factor() }
      Row {
        spacing: Noo.factor() * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
          source: Noo.pix("practice")
          height: Noo.factor() * 4; width: height * (sourceSize.width / sourceSize.height)
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: activPal.text
          text: qsTranslate("TstartExamDlg", "To exercise or to pass an exam?")
          font { bold: true; pixelSize: Noo.factor() * 1.5 }
        }
        Image {
          source: Noo.pix("exam")
          height: Noo.factor() * 4; width: height * (sourceSize.width / sourceSize.height)
        }
      }
      LinkText {
        id: exerOrExamText
        anchors.horizontalCenter: parent.horizontalCenter
        leftPadding: Noo.factor()
        width: parent.width - Noo.factor()
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        text: gotIt.exerOrExamHelpTxt()
      }
    }
  }
}
