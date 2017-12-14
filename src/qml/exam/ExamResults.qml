/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Exam 1.0
import "../"


Grid {
  id: resultsItem
  columns: 2
  width: parent.width
  y: 1; z: 10
  verticalItemAlignment: Grid.AlignVCenter; horizontalItemAlignment: Grid.AlignHCenter

  Tresults { id: results }

  Row {
    spacing: Noo.fontSize()
    width: resultsItem.width / 2; leftPadding: (width - childrenRect.width) / 2
    ResultLabel {
      width: height * 4
      score: results.answersText
    }
    ProgressBar {
      anchors.verticalCenter: parent.verticalCenter
      width: resultsItem.width / 5
      from: 0; to: results.progressMax
      value: results.progressValue
    }
    ResultLabel {
      width: height * 4
      score: results.totalText
    }
  }

  Row {
    width: resultsItem.width / 2; leftPadding: (width - childrenRect.width) / 2
    spacing: Noo.fontSize()

    ResultLabel {
      score: results.correctAnswers
      bg: GLOB.correctColor
    }
    ResultLabel {
      score: results.halfAnswers
      bg: GLOB.notBadColor
    }
    ResultLabel {
      score: results.wrongAnswers
      bg: GLOB.wrongColor
    }

    ResultLabel {
      width: height * 2.5
      score: results.effectiveness
    }
    ResultLabel {
      width: height * 2.5
      score: results.averText
    }
    ResultLabel {
      width: height * 2.5
      score: results.reactText
    }
    ResultLabel {
      width: height * 3
      score: results.totalTimeText
    }
  }

  Popup {
    id: resPop
    background: TipRect { color: activPal.button }
    y: resultsItem.height + Noo.fontSize() / 2
    Column {
      Text {
        text: "Description"
      }
    }
  }

  function more(sender) {
    resPop.parent = sender
    resPop.open()
  }

}
