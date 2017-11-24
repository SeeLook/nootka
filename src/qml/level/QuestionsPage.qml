/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Flickable {
  id: questionsPage
  height: parent.height
  contentHeight: questionCol.height + Noo.fontSize() * 2
  contentWidth: Math.max(width, Noo.fontSize() * 35)

  property var qTexts: [ qsTr("as note on the staff"), qsTr("as note name"), qsTr("on instrument"), qsTr("as played sound") ]


  Column {
    width: parent.width
    id: questionCol
    Tile {
      Flow {
        width: parent.width
        spacing: questionsPage.width / 50
        padding: questionsPage.width / 100
        Repeater {
          id: qRep
          model: 4
          QuestionsBox {
            qId: index
            questionText: qsTr("Question") + " " + qTexts[index]
            questionChecked: creator.questionAs & Math.pow(2, index)
            answerBits: creator.answersAs[index]
          }
        }
      }
    }
  }

}
