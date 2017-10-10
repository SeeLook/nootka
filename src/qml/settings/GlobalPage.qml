/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import "../"


Flickable {
  id: globalPage
  width: parent.width
  height: parent.height

  clip: true
  contentHeight: mainColumn.height + Noo.fontSize() * 2

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

  Column {
    id: mainColumn
    spacing: Noo.fontSize() / 2
    width: parent.width

    Tile {
      description: qsTranslate("TglobalSettings", "Select a language.<br><span style=\"color: red;\">To take effect, this requires restarting the application!</span>")

      ListModel {
        id: langModel
        ListElement { flag: "";  lang: QT_TR_NOOP("default") }
        ListElement { flag:"cs"; lang: "český" }
        ListElement { flag:"de"; lang: "deutsch" }
        ListElement { flag:"en"; lang: "english" }
        ListElement { flag:"es"; lang: "español" }
        ListElement { flag:"fr"; lang: "français" }
        ListElement { flag:"pl"; lang: "polski" }
        ListElement { flag:"ru"; lang: "русский" }
      }

      Tumbler {
        id: langTumb
        width: parent.width
        height: Noo.fontSize() * 9
        visibleItemCount: Math.min(((width / (Noo.fontSize() * 7)) / 2) * 2 - 1, 7)
        model: langModel
        delegate: Component {
          Column {
            spacing: Noo.fontSize() / 4
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            Image {
              source: Noo.pix("flags-" + flag)
              sourceSize.height: Noo.fontSize() * 3
              anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: flag === "" ? qsTranslate("TglobalSettings", lang) : lang
              color: activPal.text
              font.bold: langTumb.currentIndex === index
            }
          }
        }
        contentItem: PathView {
          id: pathView
          model: langTumb.model
          delegate: langTumb.delegate
          clip: true
          pathItemCount: langTumb.visibleItemCount + 1
          preferredHighlightBegin: 0.5
          preferredHighlightEnd: 0.5
          dragMargin: width / 2
          path: Path {
            startX: 0
            startY: Noo.fontSize() * 1.2
            PathLine {
              x: pathView.width
              y: Noo.fontSize() * 1.2
            }
          }
        }
        Rectangle {
          z: -1; width: Noo.fontSize() * 9; height: parent.height * 0.7
          x: parent.width / 2 - width / 2; y: -parent.height * 0.05
          color: Noo.alpha(activPal.highlight, 100)
          radius: width / 12
        }
      }
    }

    Tile {
      TcheckBox {
        id: animChBox
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("enable animations")
        checked: GLOB.useAnimations
      }
    }

    Tile {
      Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTranslate("TglobalSettings", "Check for updates")
      }
    }

    Item { height: Noo.fontSize() * 3; width: parent.width }

    Tile {
      description: qsTranslate("TglobalSettings", "All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard.")
      descriptionColor: "red"
      Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTranslate("TglobalSettings", "Restore all default settings")
      }
    }

  }

  Timer { // workaround to select 0 item, call it with delay
    id: langTimer
    running: true
    interval: 50
    onTriggered: {
      for (var i = 0; i < langModel.count; ++i) {
        if (langModel.get(i).flag === GLOB.lang) {
          langTumb.currentIndex = i
          break
        }
      }
    }
  }

  function save() {
    GLOB.useAnimations = animChBox.checked
    GLOB.lang = langModel.get(langTumb.currentIndex).flag
  }

  function defaults() {
    animChBox.checked = true
    langTumb.currentIndex = 0
  }

}
