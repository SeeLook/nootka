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

  property alias fontSize: langHead.font.pixelSize

  clip: true
  contentHeight: mainColumn.height + nootkaWindow.fontSize * 2

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

  Column {
    id: mainColumn
    spacing: nootkaWindow.fontSize / 2
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

        Column {
          width: globalPage.width
          height: langHead.height + viewItem.height

          Text {
            id: langHead;
            text: qsTranslate("TglobalSettings", "Application language")
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            color: activPal.text
          }

          Item {
            id: viewItem
            width: parent.width * 0.98
            anchors.horizontalCenter: parent.horizontalCenter
            height: fontSize * 8

            PathView {
              id: langView

              highlight: Component {
                Rectangle {
                  width: langView.currentItem.width + fontSize / 2
                  height: langView.currentItem.height + fontSize / 2
                  color: Qt.rgba(activPal.highlight.r, activPal.highlight.g, activPal.highlight.b, 0.4)
                  border { width: fontSize / 3; color: activPal.highlightedText }
                  radius: fontSize / 2
                }
              }

              model: langModel
              delegate: Component {
                Item {
                  width: Math.max(flagIcon.width, lText.width)
                  height: flagIcon.height + lText.height + fontSize
                  Column {
                    Image {
                      id: flagIcon;
                      source: Noo.pix("flags-" + flag)
                      sourceSize.height: (langView.currentIndex == index ? 7 : 4) * lText.font.pixelSize
                    }
                    Text {
                      id: lText;
                      anchors { top: flagIcon.Bottom; horizontalCenter: parent.horizontalCenter }
                      text: flag == "" ? qsTranslate("TglobalSettings", lang) : lang
                      color: activPal.text
                    }
                  }
                  MouseArea {
                    anchors.fill: parent
                    onClicked: {
                      langView.currentIndex = index
                    }
                  }
                  Component.onCompleted: {
                    if (flag === GLOB.lang )
                      langView.currentIndex = index
                  }
                }
              }
              path: Path {
                  startX: viewItem.x + fontSize * 4; startY: fontSize * 4
                  PathLine { x: viewItem.width; y: fontSize * 4 }
              }
            }
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

    Item { height: fontSize * 3; width: parent.width }

    Tile {
      description: qsTranslate("TglobalSettings", "All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard.")
      descriptionColor: "red"
      Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTranslate("TglobalSettings", "Restore all default settings")
      }
    }

  }

  function save() {
    GLOB.useAnimations = animChBox.checked
    GLOB.lang = langModel.get(langView.currentIndex).flag
  }

  function defaults() {
    animChBox.checked = true
    langView.currentIndex = 0
  }

}
