/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import Nootka.Main 1.0


GotIt {
  gotItType: TgotIt.GotExamFlow
  onClicked: close()

  Tflickable {
    anchors.fill: parent
    contentHeight: helpCol.height + Noo.factor() * 10
    Column {
      id: helpCol
      width: parent.width

      LinkText {
        anchors.horizontalCenter: parent.horizontalCenter
        leftPadding: Noo.factor()
        width: parent.width - Noo.factor()
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        text: gotIt.examHelpText()
      }
    }
  }
}
