/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  onExamSummary: nootkaWindow.showDialog(Nootka.ExamSummary)

  Connections {
    target: tipHandler
    onShowStartTip: {
      var s = Qt.createComponent("qrc:/exam/ExamTip.qml")
      s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y, "bg": color } )
    }
    onShowQuestionTip: {
      var s = Qt.createComponent("qrc:/exam/QuestionTip.qml")
      s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y } )
    }
    onShowResultTip: {
      var r = Qt.createComponent("qrc:/exam/ResultTip.qml")
      r.createObject(executor, { "text": text, "color": color } )
    }
  }
}

