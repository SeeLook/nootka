/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

// import "../"


Flickable {
  width: parent.width; height: parent.height
  clip: true
  contentHeight: questionCol.height + nootkaWindow.fontSize * 2
  contentWidth: Math.max(width, nootkaWindow.fontSize * 35)

  property var qTexts: [ qsTr("as note on the staff"), qsTr("as note name"), qsTr("as position on the fingerboard"), qsTr("as played sound") ]

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

  Column {
    width: parent.width
    id: questionCol
    Repeater {
      model: 4
      QuestionsBox {
        qId: index
        questionText: qsTr("Question") + " " + qTexts[index]
      }
    }
  }

}
