/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka.Exam 1.0
import "../"


Grid {
  id: resultsItem
  columns: 2
  width: parent.width
  height: visible ? childrenRect.height : 0
  y: 1; z: 5
  verticalItemAlignment: Grid.AlignVCenter; horizontalItemAlignment: Grid.AlignHCenter

  visible: !results.isExercise

  Tresults { id: results }

  Row {
    spacing: Noo.fontSize()
    width: resultsItem.width / 2; leftPadding: (width - childrenRect.width) / 2
    ResultLabel {
      index: 1
      width: height * 4
      score: results.answersText
      onGetTip: showTip(results.answersHint())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ProgressBar {
      id: control
      property int index: 2
      anchors.verticalCenter: parent.verticalCenter
      width: resultsItem.width / 5; height: nootkaWindow.width / 70
      from: 0; to: results.progressMax
      value: results.progressValue
      hoverEnabled: !Noo.isAndroid()

      onHoveredChanged: {
        if (GLOB.showHints) {
          if (hovered)
            Noo.setStatusTip(results.progressHint(), Item.Top)
          else
            Noo.setStatusTip("", Item.Top)
        }
      }

      background: Rectangle { // can not be null to handle mouse click/tap
        width: control.width; height: control.height * 0.8; y: control.height * 0.1
        color: "transparent"
      }

      contentItem: Item {
        width: control.width; height: control.height * 0.8; y: control.height * 0.1
        Row {
          anchors.fill: parent
          Repeater {
            model: control.width / (control.height / 2.63) // 2.63 is '!' glyph factor
            Text {
              font { family: "Nootka"; pixelSize: control.height }
              text: "!"
              color: x < control.position * control.width ? GLOB.correctColor : GLOB.wrongColor
              scale: x < control.position * control.width ? 1 : 0.8
              Behavior on color { enabled: GLOB.useAnimations; ColorAnimation { duration: 1000 } }
              Behavior on scale { enabled: GLOB.useAnimations; PropertyAnimation { duration: 1000 } }
            }
          }
        }
        Rectangle {
          id: percentRect
          width: percentText.width; height: control.height
          anchors.centerIn: parent
          color: activPal.window
          opacity: control.hovered ? 1 : 0
          Behavior on opacity { enabled: GLOB.useAnimations; PropertyAnimation {} }
          Text {
            id: percentText
            anchors.centerIn: parent
            font.pointSize: percentRect.height * 0.8
            text: " " + Math.round(control.position * 100) + " % "
          }
        }
      }

      MouseArea {
        anchors.fill: parent.background
        onClicked: resultsItem.more(parent)
      }
    }
    ResultLabel {
      index: 3
      width: height * 4
      score: results.totalText
      onGetTip: showTip(results.summaryHint())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
  }

  Row {
    width: resultsItem.width / 2; leftPadding: (width - childrenRect.width) / 2
    spacing: Noo.fontSize() / 2

    ResultLabel {
      index: 4
      score: results.correctAnswers
      onGetTip: showTip(results.correctHint())
      bg: GLOB.correctColor
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ResultLabel {
      index: 5
      score: results.halfAnswers
      onGetTip: showTip(results.halfHint())
      bg: GLOB.notBadColor
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ResultLabel {
      index: 6
      score: results.wrongAnswers
      onGetTip: showTip(results.wrongHint())
      bg: GLOB.wrongColor
      hi: popLoader.item && popLoader.item.senderIndex === index
    }

    ResultLabel {
      index: 7
      width: height * 4
      score: results.effectiveness
      onGetTip: showTip(results.effectHint())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ResultLabel {
      index: 8
      width: height * 2.5
      score: results.averText
      onGetTip: showTip(results.averageHint())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ResultLabel {
      index: 9
      width: height * 2.5
      score: results.reactText
      onGetTip: showTip(results.answerTimeTxt())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
    ResultLabel {
      index: 10
      width: height * 3
      score: results.totalTimeText
      onGetTip: showTip(results.examTimeTxt())
      hi: popLoader.item && popLoader.item.senderIndex === index
    }
  }

  Loader { id: popLoader }
  Component {
    id: popComp
    Popup {
      id: resPop
      property int senderIndex: -1
      x: (nootkaWindow.width - width) / 2
      scale: 0
      enter: Transition { enabled: GLOB.useAnimations; SpringAnimation { property: "scale"; to: 1; spring: 2; damping: 0.2; epsilon: 0.005 }}
      exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; from: 1; to: 0 }} // duration 250 ms
      background: TipRect { color: activPal.button; shadowRadius: Screen.height / 90 }
      y: resultsItem.height + Noo.fontSize() / 2
      Column {
        spacing: Noo.fontSize() / 6
        Repeater {
          model: [ results.resultsTxt(), results.answersHint(), results.progressHint(), results.summaryHint(), results.correctHint(),
                    results.halfHint(), results.wrongHint(), results.effectHint(), results.averageHint() ]
          Text {
            text: modelData
            color: resPop.senderIndex === index ? activPal.highlightedText : activPal.text
            font.bold: index === 0
            Rectangle {
              z: -1
              color: resPop.senderIndex === index ? activPal.highlight : (index % 2 === 1 ? activPal.alternateBase : activPal.base)
              width: resPop.contentWidth; height: parent.height
            }
          }
        }
        Text {
          text: results.answerTimeTxt() + ": <b>" + results.reactText + "</b>"
          color: resPop.senderIndex === 9 ? activPal.highlightedText : activPal.text
          Rectangle {
            z: -1
            color: resPop.senderIndex === 9 ? activPal.highlight : activPal.alternateBase
            width: resPop.contentWidth; height: parent.height
          }
        }
        Text {
          text: results.examTimeTxt() + ": <b>" + results.totalTimeText + "</b>"
          color: resPop.senderIndex === 10 ? activPal.highlightedText : activPal.text
          Rectangle {
            z: -1
            color: resPop.senderIndex === 10 ? activPal.highlight : activPal.base
            width: resPop.contentWidth; height: parent.height
          }
        }
      }
      onClosed: popLoader.sourceComponent = null
    }
  }

  function more(sender) {
    popLoader.sourceComponent = popComp
//     popLoader.item.parent = sender
    popLoader.item.open()
    popLoader.item.senderIndex = sender.index
  }

}
