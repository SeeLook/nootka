/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import Score 1.0
import "../"


Column {

    spacing: nootkaWindow.fontSize
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
    StackLayout {
      id: swipePages
      height: parent.height - headList.height - nootkaWindow.fontSize
      width: parent.width

      Flickable { // 1st page (general)
        clip: true
        width: parent.width
        height: parent.height
        contentHeight: firstColumn.height + nootkaWindow.fontSize * 2
        contentWidth: Math.max(width, nootkaWindow.fontSize * 35)
        Column {
          id: firstColumn
          width: parent.width
          spacing: nootkaWindow.fontSize / 2
          Frame {
            width: parent.width
            Column {
              spacing: nootkaWindow.fontSize / 2
              width: parent.width
              Tile {
                description: qsTranslate("TscoreSettings", "When enabled, a score displays only a single note.")
                TcheckBox {
                  id: singleNoteModeChB
                  text: qsTranslate("TscoreSettings", "use single note only")
                  anchors.horizontalCenter: parent.horizontalCenter
                }
              }

              Tile {
                enabled: singleNoteModeChB.checked
                description: qsTranslate("TscoreSettings",
                                        "Shows enharmonic variants of notes.<br>i.e.: the note E is also Fb (F flat) <i>and</i> Dx (D with double sharp).")
                TcheckBox {
                  id: showEnharmNotesChB
                  text: qsTranslate("TscoreSettings", "show enharmonic variants of notes")
                  anchors.horizontalCenter: parent.horizontalCenter
                }
              }
              Tile {
                enabled: singleNoteModeChB.checked
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
            description: qsTr("Use rhythms or only bare note heads.")
            enabled: !singleNoteModeChB.checked
            TcheckBox {
              id: rhythmsEnabledChB
              text: qsTr("enable rhythms")
              anchors.horizontalCenter: parent.horizontalCenter
              checked: GLOB.rhythmsEnabled
            }
          }
          Tile {
            description: qsTranslate("TscoreSettings", "If checked, you can use double sharps and double flats.")
            TcheckBox {
              id: doubleAccidsChB
              text: qsTranslate("TscoreSettings", "use double accidentals")
              anchors.horizontalCenter: parent.horizontalCenter
              checked: GLOB.enableDoubleAccids
            }
          }
          Tile {
            Row {
              spacing: nootkaWindow.fontSize
              anchors.horizontalCenter: parent.horizontalCenter
              Text { color: activPal.text; text: qsTranslate("TscoreSettings", "note-cursor color"); anchors.verticalCenter: parent.verticalCenter }
              ColorButton { id: pointerColorButt; color: GLOB.noteCursorColor }
            }
          }
        }
        Component.onCompleted: {
          enharmNoteColor.color = GLOB.enharmNoteColor
          showEnharmNotesChB.checked = GLOB.showEnharmNotes
          singleNoteModeChB.checked = GLOB.singleNoteMode
        }

        ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

      }

      Flickable { // 2nd page (key signatures)
        clip: true
        width: parent.width
        height: parent.height
        contentHeight: secondColumn.height + nootkaWindow.fontSize * 2
        contentWidth: Math.max(width, nootkaWindow.fontSize * 35)
        Column {
          id: secondColumn
          width: parent.width
          spacing: nootkaWindow.fontSize / 2
          TcheckBox {
            id: enableKeyChB
            text: qsTranslate("TscoreSettings", "enable key signature")
            anchors.horizontalCenter: parent.horizontalCenter
            checked: GLOB.keySignatureEnabled
          }
          Frame {
            width: parent.width * 0.96
            anchors.horizontalCenter: parent.horizontalCenter
            Column {
              spacing: nootkaWindow.fontSize / 2
              width: parent.width
              Tile {
                enabled: enableKeyChB.checked
                TcheckBox {
                  id: showKeyNamesChB
                  text: qsTranslate("TscoreSettings", "show names of key signature")
                  anchors.horizontalCenter: parent.horizontalCenter
                  checked: GLOB.showKeyName
                }
              }

              Tile {
                enabled: enableKeyChB.checked && showKeyNamesChB.checked
                Grid {
                  columns: parent.width < nootkaWindow.fontSize * 50 ? 1 : 2
                  spacing: nootkaWindow.fontSize
                  anchors.horizontalCenter: parent.horizontalCenter
                  horizontalItemAlignment: Grid.AlignHCenter
                  NameStyleSelector {
                    id: keyNameStyleSel
                    seventhIsB: GLOB.seventhIsB
                  }
                  Column {
                    Text {
                      text: qsTranslate("TscoreSettings", "Naming extension")
                      anchors.horizontalCenter: parent.horizontalCenter
                      color: enabled ? activPal.text : disdPal.text
                    }
                    Row {
                      spacing: nootkaWindow.fontSize * 2
                      anchors.horizontalCenter: parent.horizontalCenter
                      KeySufixEdit { id: majKeySufixText; nameStyle: keyNameStyleSel.style }
                      KeySufixEdit { id: minKeySufixText; nameStyle: keyNameStyleSel.style; noteOne: 1; alterOne: 1; noteTwo: 5; alterTwo: 0 }
                    }
                  }
                }
                Component.onCompleted: {
                  majKeySufixText.sufix = GLOB.majorKeyNameSufix
                  minKeySufixText.sufix = GLOB.minorKeyNameSufix
                  keyNameStyleSel.style = GLOB.keyNameStyle
                }
              }
            }
          }
        }

        ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

      }

      Flickable { // 3rd page (clefs)
        clip: true
        width: parent.width
        contentHeight: clefsCol.height + nootkaWindow.fontSize * 2
        contentWidth: Math.max(width, nootkaWindow.fontSize * 35)
        Column {
          id: clefsCol
          width: parent.width
          spacing: nootkaWindow.fontSize / 2
          Text {
            text: qsTranslate("TscoreSettings", "Select default clef for the application.") + "<br><b>" + qsTranslate("TscoreSettings", "Remember! Not all clefs are suitable for some possible tunings or instrument types!") + "<b>"
            textFormat: Text.StyledText
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
            wrapMode: Text.WordWrap
          }
          ClefMenu {
            id: clefs
            columns: width < nootkaWindow.fontSize * 40 ? 1 : 2
            Component.onCompleted: selClef = GLOB.clefType
          }
        }

        ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

      }

      Flickable { // 4rd page (note name calling)
        clip: true
        width: parent.width
        contentHeight: nameCol.height + nootkaWindow.fontSize * 2
        contentWidth: Math.max(width, nootkaWindow.fontSize * 35)
        Column {
          id: nameCol
          width: parent.width
          spacing: nootkaWindow.fontSize / 2
          anchors.horizontalCenter: parent.horizontalCenter
          Tile {
            description: qsTranslate("NameStyleSelector", "Naming style of note. The main difference is the 7th note.<br>Is it B and B flat, or H and B?")
            Column {
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: nootkaWindow.fontSize * 2
              Select7note {
                id: is7BSelector
                anchors.horizontalCenter: parent.horizontalCenter
                style: nameStyleSel.style
              }
              NameStyleSelector {
                id: nameStyleSel
                seventhIsB: is7BSelector.is7B
                anchors.horizontalCenter: parent.horizontalCenter
              }
            }
          }
          Tile {
            TcheckBox {
              id: namesOnScoreChB
              text: qsTr("Show names of all notes on the score.")
              anchors.horizontalCenter: parent.horizontalCenter
              checked: GLOB.namesOnScore
            }
          }
          Tile {
            enabled: namesOnScoreChB.checked
            Row {
              spacing: nootkaWindow.fontSize
              anchors.horizontalCenter: parent.horizontalCenter
              Text { color: enabled ? activPal.text : disdPal.text; text: qsTr("names highlight color"); anchors.verticalCenter: parent.verticalCenter }
              ColorButton { id: nameColorButt; color: GLOB.nameColor }
            }
          }
          Component.onCompleted: {
            nameStyleSel.style = GLOB.noteNameStyle
            is7BSelector.is7B = GLOB.seventhIsB
          }
        }

        ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

      }
    }

    function save() {
//       GLOB. = singleNoteModeChB
      GLOB.rhythmsEnabled = rhythmsEnabledChB.checked
      GLOB.enableDoubleAccids = doubleAccidsChB.checked
      GLOB.noteCursorColor = pointerColorButt.color
      GLOB.keySignatureEnabled = enableKeyChB.checked
      if (GLOB.keySignatureEnabled) {
        GLOB.showKeyName = showKeyNamesChB.checked
        if (GLOB.showKeyName) {
          GLOB.majorKeyNameSufix = majKeySufixText.sufix
          GLOB.minorKeyNameSufix = minKeySufixText.sufix
          GLOB.keyNameStyle = keyNameStyleSel.style
          GLOB.updateKeySignatureNames()
        }
      }
      GLOB.noteNameStyle = nameStyleSel.style
      GLOB.seventhIsB = is7BSelector.is7B
      GLOB.namesOnScore = namesOnScoreChB.checked
      GLOB.nameColor = nameColorButt.color
    }

    function defaults() {
      singleNoteModeChB.checked = false
      showEnharmNotesChB.checked = false
      enharmNoteColor.color = Qt.rgba(0, 0.6352941176470588, 0.6352941176470588, 1)
      doubleAccidsChB.checked = false
      pointerColorButt.color = "pink"

      enableKeyChB.checked = false
    }
}
