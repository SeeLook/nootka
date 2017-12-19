/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import Nootka.Exam 1.0

import "../"


Texecutor {
  id: executor

  anchors.fill: parent

  onTitleChanged: nootkaWindow.title = title
  onExamActionsChanged: nootkaWindow.mainMenu.toolBar.examActions = examActions

  Connections {
    target: tipHandler
    onShowStartTip: {
      var s = Qt.createComponent("qrc:/exam/ExamTip.qml")
      s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y, "bg": color } )
    }
  }
}

