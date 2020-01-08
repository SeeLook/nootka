/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0
import "../"


Rectangle {
  property string helpText: dialLoader.dialogObj.mainHelp()
  property alias enableTOC: tocButt.visible

  width: parent.width; height: parent.height
  color: Noo.alpha(activPal.base, 240)
  z: 0
  Image {
    source: Noo.pix(images[currTopic])
    height: parent.height; width: height * (sourceSize.width / sourceSize.height)
    z: -1
    anchors.horizontalCenter: parent.horizontalCenter
  }

  TcuteButton {
    id: tocButt
    y: Noo.fontSize() / 4; x: Noo.fontSize()
    text: qsTr("Help topics")
    onClicked: tocMenu.open()
  }

  Tflickable {
    y: (tocButt.visible ? tocButt.height : 0) + Noo.fontSize()
    width: parent.width; height: parent.height - y
    contentHeight: text.height;

    LinkText {
      id: text
      width: parent.width
      padding: Noo.fontSize()
      wrapMode: TextEdit.Wrap; textFormat: Text.RichText
      text: helpText
    }
  }

  property var topics: [ qsTranslate("ThelpDialogBase", "Nootka help"),
                          qsTranslate("TstartExamDlg", "To exercise or to pass an exam?"),
                          qsTranslate("TexamHelp", "How does an exercise or an exam work?"),
                          qsTranslate("ThelpDialogBase", "Open online documentation")
                       ]
  property var images: [ "help", "startExam", "nootka-exam", "restore-defaults" ]
  property int currTopic: 0

  Tmenu {
    id: tocMenu
    width: Noo.fontSize() * 30
    x: tocButt.width; y: Noo.fontSize() / 2
    Repeater {
      model: topics
      MenuItem {
        padding: 0
        contentItem: MenuButton {
          action: Taction {
            text: (currTopic === index ? "<u>" : "") + modelData + (currTopic === index ? "</u>" : "")
            icon: images[index]
          }
          onClicked: switchTopic(index)
          Rectangle { width: parent.width; height: index === topics.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1 }
        }
      }
    }
  }

  function switchTopic(tp) {
    if (tp !== currTopic) {
      if (tp === topics.length - 1) {
          Qt.openUrlExternally("https://nootka.sourceforge.io/index.php/help/")
      } else {
          switch (tp) {
            case 1: helpText = dialLoader.dialogObj.exerOrExamHelp(); break
            case 2: helpText = dialLoader.dialogObj.examHelp(); break
            default: helpText = dialLoader.dialogObj.mainHelp(); break
          }
          currTopic = tp
      }
    }
    tocMenu.close()
  }
}
