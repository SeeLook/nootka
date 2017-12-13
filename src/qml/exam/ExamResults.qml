/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Exam 1.0


Grid {
  columns: 2
  width: parent.width
  y: 1; z: 10

  Tprogress {
    id: progress
    height: row1.height; width: parent.width / 2
    Row {
      id: row1
      spacing: Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter
      ResultLabel {
        width: Noo.fontSize() * 6
        score: progress.answersText
      }
      ProgressBar {
        anchors.verticalCenter: parent.verticalCenter
        width: progress.width / 2
        from: 0; to: progress.progressMax
        value: progress.progressValue
      }
      ResultLabel {
        width: Noo.fontSize() * 6
        score: progress.totalText
      }
    }
  }

  Tresults {
    id: results
    height: row2.height; width: parent.width / 2
    Row {
      id: row2
      spacing: Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter

      ResultLabel {
        score: results.correctAnswers
        color: "green"
      }
      ResultLabel {
        score: results.halfAnswers
        color: "yellow"
      }
      ResultLabel {
        score: results.wrongAnswers
        color: "red"
      }

      ResultLabel {
        width: Noo.fontSize() * 4
        score: results.effectiveness
      }
      ResultLabel {
        width: Noo.fontSize() * 4
        score: results.averText
      }
      ResultLabel {
        width: Noo.fontSize() * 6
        score: results.reactText
      }
      ResultLabel {
        width: Noo.fontSize() * 6
        score: results.totalTimeText
      }
    }
  }

}
