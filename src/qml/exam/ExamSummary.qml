/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

import Nootka 1.0
import Nootka.Exam 1.0
import "../level"
import "../"


TexamSummary {
  id: summDialog
  width: parent.width; height: parent.height
  levelPreview: previewItem

  Column {
    width: parent.width
    spacing: NOO.factor() / 2

    Tflickable {
      width: parent.width; height: summDialog.height - buttGrid.height - NOO.factor()
      clip: true
      contentWidth: width; contentHeight: resGrid.height
      Grid {
        id: resGrid
        columns: summDialog.width > NOO.factor() * 50 ? 2 : 1
        spacing: NOO.factor() / 2
        anchors.horizontalCenter: parent.horizontalCenter

        Column {
          width: summDialog.width / resGrid.columns - NOO.factor() / 2
          Text { // student
            anchors.horizontalCenter: parent.horizontalCenter
            text: student; textFormat: Text.StyledText
          }
          Item { width: NOO.factor(); height: NOO.factor() }
          Tile { // answers/mistakes numbers
            width: parent.width
            Grid {
              anchors.horizontalCenter: parent.horizontalCenter
              columns: NOO.isAndroid() || parent.width < NOO.factor() * 50 ? 1 : 2
              horizontalItemAlignment: Grid.AlignHCenter; verticalItemAlignment: Grid.AlignVCenter
              spacing: NOO.factor() * (NOO.isAndroid() ? 1 : 2)
              Column {
                spacing: NOO.factor()
                Text {
                  anchors.horizontalCenter: parent.horizontalCenter
                  horizontalAlignment: Text.AlignHCenter
                  text: resultHeader; textFormat: Text.StyledText
                  font.pixelSize: NOO.factor() * 1.3
                }
                Grid {
                  columns: 2; columnSpacing: NOO.factor()
                  Repeater {
                    model: answersLabel
                    Text {
                      text: modelData
                      color: activPal.text; textFormat: Text.StyledText
                      font.pixelSize: NOO.factor() * 1.2
                    }
                  }
                }
              }
              Item {
                width: NOO.factor() * (NOO.isAndroid() ? 15 : 20); height: width + NOO.factor()
                visible: hasQuestions
                TpieChartItem {
                  id: answId
                  width: parent.width; height: width
                  values: answersModel
                  colors: [ cn(GLOB.correctColor), cn(GLOB.notBadColor), cn(GLOB.wrongColor) ]
                }
                DropShadow {
                  anchors.fill: answId
                  horizontalOffset: NOO.factor() / 2; verticalOffset: NOO.factor() / 2
                  radius: NOO.factor()
                  samples: 1 + radius * 2; color: activPal.shadow
                  source: answId
                }
              }
            }
          }
          Tile { // results
            width: parent.width
            visible: resultsModel.length
            Grid {
              anchors.horizontalCenter: parent.horizontalCenter
              columns: NOO.isAndroid() || parent.width < NOO.factor() * 50 ? 1 : 2
              horizontalItemAlignment: Grid.AlignHCenter; verticalItemAlignment: Grid.AlignVCenter
              spacing: NOO.factor() * (NOO.isAndroid() ? 1 : 2)
              Column {
                spacing: NOO.factor()
                Text {
                  anchors.horizontalCenter: parent.horizontalCenter
                  text: qsTranslate("TexamSummary", "Kinds of mistakes") + ":"; color: activPal.text
                }
                Grid {
                  columns: 2; columnSpacing: NOO.factor()
                  Repeater {
                    model: resultsModel
                    Text {
                      text: modelData
                      color: activPal.text; textFormat: Text.StyledText
                    }
                  }
                }
              }
              Item {
                visible: hasVariousMistakes
                width: NOO.factor() * (NOO.isAndroid() ? 15 : 20); height: width + NOO.factor()
                TpieChartItem {
                  id: pie
                  width: parent.width; height: width
                  values: summDialog.kindOfMistakes
                  colors: [ cn(GLOB.wrongColor) ]
                }
                DropShadow {
                  anchors.fill: pie
                  horizontalOffset: NOO.factor() / 2; verticalOffset: NOO.factor() / 2
                  radius: NOO.factor()
                  samples: 1 + radius * 2; color: activPal.shadow
                  source: pie
                }
              }
            }
          }
        }

        Column {
          width: summDialog.width / resGrid.columns - NOO.factor() / 2
          Tile { // times
            id: timeTile
            width: parent.width
            Column {
              spacing: NOO.factor()
              width: parent.width
              Text {
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                text: qsTranslate("TexamSummary", "times:")
                font.pixelSize: NOO.factor() * 1.1
              }
              Grid {
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 2; columnSpacing: NOO.factor()
                Repeater {
                  model: timesModel
                  Text {
                    text: modelData
                    color: activPal.text; textFormat: Text.StyledText
                  }
                }
              }
            }
          }
          LevelPreview {
            id: previewItem
            width: parent.width
            height: resGrid.columns === 2 ? summDialog.height - buttGrid.height - NOO.factor() - timeTile.height : summDialog.height / 2
          }
        }
      } // resGrid
    } // Tflickable

    Grid {
      id: buttGrid
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: summDialog.width / 100
      columns: buttColumsCount()
      property real buttWidth: buttColumsCount() === 2 ? summDialog.width / 3 : summDialog.width / 4
      property real buttHeight: Math.min(summDialog.width, summDialog.height) / 15
        TiconButton {
          visible: enableContinue()
          width: buttGrid.buttWidth
          pixmap: NOO.pix(isExercise() ? "practice" : "exam"); iconHeight: buttGrid.buttHeight
          text: NOO.TR("QWizard", "Continue")
          onClicked: { continueExecutor(); dialLoader.close() }
        }
        TiconButton {
          visible: hasQuestions && (!NOO.isAndroid() || !isExercise())
          width: buttGrid.buttWidth
          pixmap: NOO.pix(NOO.isAndroid() ? "send" : "charts"); iconHeight: buttGrid.buttHeight
          text: NOO.isAndroid() ? NOO.TR("QShortcut", "Send") : qsTr("Analyze")
          onClicked: {
            if (NOO.isAndroid()) {
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
          pixmap: NOO.pix("exam"); iconHeight: buttGrid.buttHeight
          text: qsTr("Pass an exam")
          onClicked: { exerciseToExam(); dialLoader.close() }
        }
        TiconButton {
          width: buttGrid.buttWidth
          pixmap: NOO.pix("exit"); iconHeight: buttGrid.buttHeight
          text: NOO.TR("QPlatformTheme", "Close")
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
