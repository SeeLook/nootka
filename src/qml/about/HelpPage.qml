/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0
import "../"


Rectangle {
  property string helpText: mainHelpText /** @p mainHelpText has to be defined somewhere above */
  property alias enableTOC: tocButt.visible

  width: parent.width; height: parent.height
  color: NOO.alpha(activPal.base, 240)
  z: 0
  Image {
    source: currTopic ? "" : NOO.pix("help")
    height: parent.height * Math.min(1, (parent.width / parent.height) / (sourceSize.width / sourceSize.height))
    width: height * (sourceSize.width / sourceSize.height)
    z: -1
    anchors.centerIn: parent
  }

  TcuteButton {
    id: tocButt
    y: NOO.factor() / 4; x: parent.width - width - NOO.factor()
    z: 10
    text: qsTr("Help topics").replace(" ", "\n")
    onClicked: tocMenu.open()
  }

  StackView {
    id: stack
    anchors.fill: parent
    initialItem: mainHelp
    replaceEnter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "y"; from: -height; to: 0 }}
    replaceExit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "y"; from: 0; to: height }}
  }

  Component {
    id: mainHelp
    Tflickable {
      y: (tocButt.visible ? tocButt.height : 0) + NOO.factor()
      width: parent ? parent.width : 0; height: parent ? parent.height - y : 0
      contentHeight: text.height;
      LinkText {
        id: text
        width: parent.width
        padding: NOO.factor()
        wrapMode: TextEdit.Wrap; textFormat: Text.RichText
        text: helpText
      }
    }
  }

  property var topics: [ qsTranslate("ThelpDialogBase", "Nootka help"),
                          qsTranslate("TstartExamDlg", "To exercise or to pass an exam?"),
                          qsTranslate("TexamHelp", "How does an exercise or an exam work?"),
                          NOO.TR("MainMenuMobile", "Pitch recognition"),
                          NOO.TR("NoteSelected", "Note selection and playing"),
                          NOO.TR("HandleScore", "Editing score with touch"),
                          qsTranslate("ThelpDialogBase", "Open online documentation")
                       ]
  property int currTopic: 0
  property var gotItQML: [ "", "ExamOrExercise", "ExamFlow", "SoundInfo", "NoteSelected", "HandleScore" ]
  property var gotItObj: [ null,    null,           null,       null,         null,           null      ]

  Component.onCompleted: {
    if (enableTOC)
      tocRep.model = topics
  }

  Tmenu {
    id: tocMenu
    width: NOO.factor() * 25
    x: parent.width - width - y; y: NOO.factor() / 2
    Repeater {
      id: tocRep
      MenuItem {
        padding: 0
        contentItem: MenuButton {
          action: Taction {
            text: (currTopic === index ? "<u>" : "") + modelData + (currTopic === index ? "</u>" : "")
            icon: index ? "" : "help"
          }
          onClicked: switchTopic(index)
          Rectangle { width: parent.width; height: index === topics.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
        }
      }
    }
  }

  onVisibleChanged: { // stop/start animation when user switches to/from another About page
    if (currTopic > 0 && currTopic < topics.length - 1) {
      if (visible)
        gotItObj[currTopic].start()
        else
          gotItObj[currTopic].stop()
    }
  }

  function switchTopic(tp) {
    if (tp !== currTopic) {
      if (tp === topics.length - 1) {
          Qt.openUrlExternally("https://nootka.sourceforge.io/index.php/help/")
      } else {
          if (tp > 0) {
              if (!gotItObj[tp])
                gotItObj[tp] = Qt.createComponent("qrc:/gotit/" + gotItQML[tp] + ".qml").createObject(stack, { "visible": false, "showGotIt": false })
              else
                gotItObj[tp].start()
              stack.replace(gotItObj[tp].contentItem)
              if (currTopic > 0)
                gotItObj[currTopic].stop()
          } else
              stack.replace(mainHelp)
          currTopic = tp
      }
    }
    tocMenu.close()
  }
}
