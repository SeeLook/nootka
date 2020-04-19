/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Exam 1.0
import "../level"
import "../"


TexamSummary {
  id: summDialog
  width: parent.width; height: parent.height
  levelPreview: previewItem

  Column {

    Grid {
      columns: 2
      padding: summDialog.width / 200
      Tflickable {
        height: summDialog.height - buttGrid.height - summDialog.width / 50; width: summDialog.width * 0.49
        contentHeight: summCol.height
        Column {
          id: summCol
          spacing: summDialog.width / 200
          width: parent.width
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: student; textFormat: Text.StyledText
          }
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
//             width: parent.width * 0.9
            text: results; textFormat: Text.RichText
          }
          TpieChartItem {
            visible: hasVariousMistakes
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width / 2; height: width
            values: summDialog.kindOfMistakes
          }
        }
      }
      LevelPreview {
        id: previewItem
        width: summDialog.width * 0.49; height: summDialog.height - buttGrid.height - summDialog.width / 50
      }
    }
    Grid {
      id: buttGrid
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: summDialog.width / 100
      columns: buttColumsCount()
      property real buttWidth: buttColumsCount() === 2 ? summDialog.width / 3 : summDialog.width / 4
        TiconButton {
          visible: enableContinue()
          width: buttGrid.buttWidth
          pixmap: Noo.pix(isExercise() ? "practice" : "exam"); iconHeight: summDialog.height / 15
          text: Noo.TR("QWizard", "Continue")
          onClicked: { continueExecutor(); dialLoader.close() }
        }
        TiconButton {
          visible: !Noo.isAndroid() || !isExercise()
          width: buttGrid.buttWidth
          pixmap: Noo.pix(Noo.isAndroid() ? "send" : "charts")
          iconHeight: summDialog.height / 15
          text: Noo.isAndroid() ? Noo.TR("QShortcut", "Send") : qsTr("Analyze")
          onClicked: {
            if (Noo.isAndroid()) {
                sendExam()
            } else {
                nootkaWindow.showDialog(6) // 6 - Nootka.Charts
                nootkaWindow.analyzeWindow.allowOpen = false
                nootkaWindow.analyzeWindow.exam = summDialog.exam()
            }
          }
        }
        TiconButton {
          visible: isExercise()
          width: buttGrid.buttWidth
          pixmap: Noo.pix("exam"); iconHeight: summDialog.height / 15
          text: qsTr("Pass an exam")
          onClicked: { exerciseToExam(); dialLoader.close() }
        }
        TiconButton {
          width: buttGrid.buttWidth
          pixmap: Noo.pix("exit"); iconHeight: summDialog.height / 15
          text: Noo.TR("QPlatformTheme", "Close")
          onClicked: dialLoader.close()
        }
    }
  }

  Component.onCompleted: {
    dialLoader.standardButtons = 0
    dialLoader.title = summDialog.title()
  }

  Component.onDestruction: {
    if (discardedAtBegin()) {
        nootkaWindow.executor.destroy()
        GLOB.isExam = false
    } else
        closeSummary()
  }
}
