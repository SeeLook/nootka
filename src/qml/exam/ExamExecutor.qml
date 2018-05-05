/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import Nootka.Exam 1.0

import "../"


Texecutor {
  id: executor

  //private
  property var examSettDialog: null

  anchors.fill: parent

  onTitleChanged: nootkaWindow.title = title
  onExamActionsChanged: nootkaWindow.mainMenu.toolBar.examActions = examActions
  onExamSummary: nootkaWindow.showDialog(Nootka.ExamSummary)
  onShowSettings: {
    if (!examSettDialog) {
      var e = Qt.createComponent("qrc:/exam/ExamSettingsDialog.qml")
      examSettDialog =  e.createObject(executor, { "mode": isExercise ? 2 : 1 } )
      examSettDialog.accepted.connect(settingsAccepted)
      examSettDialog.closed.connect(function() { examSettDialog.destroy() })
    }
  }

  Connections {
    target: tipHandler
    onWantStartTip: {
      var s = Qt.createComponent("qrc:/exam/ExamTip.qml")
      tipHandler.startTip = s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y, "bg": color } )
    }
    onWantQuestionTip: {
      var s = Qt.createComponent("qrc:/exam/QuestionTip.qml")
      tipHandler.questionTip = s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y } )
    }
    onWantConfirmTip: {
      var s = Qt.createComponent("qrc:/exam/ExamTip.qml")
      tipHandler.confirmTip = s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y, "bg": color } )
    }
    onWantResultTip: {
      var r = Qt.createComponent("qrc:/exam/ResultTip.qml")
      tipHandler.resultTip = r.createObject(executor, { "text": text, "color": color } )
    }
    onWantTryAgainTip: {
      var a = Qt.createComponent("qrc:/exam/ResultTip.qml")
      tipHandler.tryAgainTip = a.createObject(executor, { "text": qsTranslate("TtipHandler", "Try again!"),
                                                            "color": GLOB.wrongColor, "targetY": executor.height / 10 } )
    }
    onWantWhatNextTip:  {
      var s = Qt.createComponent("qrc:/exam/ExamTip.qml")
      tipHandler.whatNextTip = s.createObject(executor, { "text": text, "offX": pos.x, "offY": pos.y, "bg": color } )
    }
    onWantCertificate: {
      var c = Qt.createComponent("qrc:/exam/Certificate.qml")
      tipHandler.certTip = c.createObject(nootkaWindow.contentItem.parent)
    }
  }
}

