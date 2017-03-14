/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3


Column {

    spacing: nootkaWindow.fontSize

    Row {
      width: parent.width

      ListView {
        id: headList
        orientation: ListView.Horizontal
        spacing: nootkaWindow.fontSize
        width: parent.width

        model: ListModel {
          ListElement { head: QT_TR_NOOP("Score settings") }
          ListElement { head: QT_TR_NOOP("Key signatures") }
          ListElement { head: QT_TR_NOOP("Clefs") }
          ListElement { head: QT_TR_NOOP("Notes naming") }
        }

        delegate: Component {
          Button {
            text: (index + 1) + ". " + qsTranslate("TscoreSettings", head)
            onClicked: { swipePages.currentIndex = index; headList.currentIndex = index }
            highlighted: headList.currentIndex == index
            Component.onCompleted: headList.height = Math.max(height, headList.height)
          }
        }
      }
    }
    StackLayout {
      id: swipePages
      height: parent.height - headList.height
      width: parent.width

      Item { // 1st page
        Column {
          anchors.fill: parent
          spacing: nootkaWindow.fontSize / 2
          Frame {
            width: parent.width
            Column {
              spacing: nootkaWindow.fontSize / 2
              width: parent.width
              Tile {
                description: qsTranslate("TscoreSettings", "When enabled, a score displays only a single note.")
                CheckBox {
                  id: singleNoteMode
                  text: qsTranslate("TscoreSettings", "use single note only")
                  anchors.horizontalCenter: parent.horizontalCenter
                }
              }

              Tile {
                enabled: singleNoteMode.checked
                width: parent.width * 0.95
                anchors.horizontalCenter: parent.horizontalCenter
                description: qsTranslate("TscoreSettings",
                                        "Shows enharmonic variants of notes.<br>i.e.: the note E is also Fb (F flat) <i>and</i> Dx (D with double sharp).")
                CheckBox {
                  id: showEnharmNotes
                  text: qsTranslate("TscoreSettings", "show enharmonic variants of notes")
                  anchors.horizontalCenter: parent.horizontalCenter
                }
              }
              Tile {
                enabled: singleNoteMode.checked
                width: parent.width * 0.95
                anchors.horizontalCenter: parent.horizontalCenter
                Row {
                  spacing: nootkaWindow.fontSize
                  anchors.horizontalCenter: parent.horizontalCenter
                  Text {
                    color: enabled ? activPal.text : disdPal.text
                    text: qsTranslate("TscoreSettings", "color of enharmonic notes")
                    anchors.verticalCenter: parent.verticalCenter
                  }
                  ColorButton { id: enharmNoteColor }
                }
              }
            }
          }
          Tile {
            Row {
              spacing: nootkaWindow.fontSize
              anchors.horizontalCenter: parent.horizontalCenter
              Text { color: activPal.text; text: qsTranslate("TscoreSettings", "note-cursor color"); anchors.verticalCenter: parent.verticalCenter }
              ColorButton { id: pointerColor }
            }
          }
        }
        Component.onCompleted: {
          enharmNoteColor.color = GLOB.enharmNoteColor
          showEnharmNotes.checked = GLOB.showEnharmNotes
          singleNoteMode.checked = GLOB.singleNoteMode
        }
      }

      Flickable { // 2nd page (key signatures)
        clip: true
        contentHeight: childrenRect.height
        Column {
          id: secondColumn
          anchors.fill: parent
          spacing: nootkaWindow.fontSize / 2
          Tile {
            anchors.horizontalCenter: parent.horizontalCenter
            CheckBox {
              id: enableKeyChB
              text: qsTranslate("TscoreSettings", "enable key signature")
              anchors.horizontalCenter: parent.horizontalCenter
            }
          }
          Frame {
            width: parent.width
            Column {
              spacing: nootkaWindow.fontSize / 2
              width: parent.width
              Tile {
                enabled: enableKeyChB.checked
                anchors.horizontalCenter: parent.horizontalCenter
                CheckBox {
                  id: showKeyNamesChB
                  text: qsTranslate("TscoreSettings", "show names of key signature")
                  anchors.horizontalCenter: parent.horizontalCenter
                }
              }

              Tile {
                enabled: enableKeyChB.checked && showKeyNamesChB.checked
                width: parent.width * 0.95
                anchors.horizontalCenter: parent.horizontalCenter
                Column {
                  spacing: nootkaWindow.fontSize / 2
                  width: parent.width
                  RadioButton {
                    text: qsTr("Scandinavian")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                  RadioButton {
                    text: qsTr("Italian")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                  RadioButton {
                    text: qsTr("German")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                  RadioButton {
                    text: qsTr("English")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                  RadioButton {
                    text: qsTr("Dutch")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                  RadioButton {
                    text: qsTr("Russian")
//                     anchors.horizontalCenter: parent.horizontalCenter
                  }
                }
              }
            }
          }
        }
      }
      Item {
        Text {
          text: "Clefs"
          anchors.centerIn: parent
        }
      }
      Item {
        Text {
          text: "Note names"
          anchors.centerIn: parent
        }
      }
    }

    function save() {
      
    }

    function defaults() {
      singleNoteMode.checked = false
      showEnharmNotes.checked = false
      enharmNoteColor.color = Qt.rgba(0, 0.6352941176470588, 0.6352941176470588, 1)
      pointerColor.color = "pink"
    }
}
