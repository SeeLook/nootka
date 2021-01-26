/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  id: levelsPage

  height: parent.height; width: parent.width

  spacing: levelsPage.width / 50
  LevelsSelector {
    id: selector
    width: parent.width
    height: levelsPage.height - buttRow.height - levelsPage.width / 25 - (Noo.isAndroid() ? 0 : Noo.fontSize() * 3)
    Component.onCompleted: creator.selector = selector
  }
  Row {
    id: buttRow
    spacing: levelsPage.width / 50
    anchors.horizontalCenter: parent.horizontalCenter
    TiconButton {
      enabled: creator.notSaved
      width: levelsPage.width / 4
      pixmap: Noo.pix("pane/notSaved"); iconHeight: levelsPage.height / 15
      text: Noo.TR("QShortcut", "Save")
      onClicked: creator.saveLevel()
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("exam"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exam")
      onClicked: start(2, selector.currentLevelVar())
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: Noo.pix("practice"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exercise")
      onClicked: start(4, selector.currentLevelVar())
    }
  }
  LinkText {
    visible: !Noo.isAndroid()
    text: qsTr("Get more levels <a href=\"%1\">from Nootka home page</a>").arg("https://nootka.sourceforge.io/index.php/download/#levels")
    anchors.horizontalCenter: parent.horizontalCenter
  }

  function start(action, argument) {
    GLOB.isExam = true
    if (!nootkaWindow.executor.init(action, argument)) {
        console.log("Executor discarded, deleting it")
        nootkaWindow.executor.destroy()
        GLOB.isExam = false
    }
    dialLoader.close()
  }

  Connections {
    target: creator
    onSaveNewLevel: {
      newLevelPopup.name = name === "" ? qsTr("new level") : name
      newLevelPopup.desc = desc
      newLevelPopup.open()
    }
  }

  TpopupDialog {
    id: newLevelPopup
    property alias name: lName.text
    property alias desc: lDesc.text

    modal: true
    width: Noo.fontSize() * 40; height: popCol.height + Noo.fontSize() * 5
    x: (parent.width - width) / 2
    y: Noo.isAndroid() ? Noo.fontSize() : (parent.height - height) / 2
    bgColor: Qt.tint(activPal.window, Noo.alpha(activPal.highlight, 30))
    border { color: activPal.highlight; width: Noo.fontSize() / 4.0 }

    rejectButton.text: Noo.TR("QPlatformTheme", "Discard")
    acceptButton.text: Noo.TR("QShortcut", "Save")
    acceptButton.pixmap: Noo.pix("pane/notSaved")

    Column {
      id: popCol
      anchors.centerIn: parent
      spacing: Noo.fontSize()
      Row {
        anchors { right: parent.right; rightMargin: Noo.fontSize() / 2 }
        spacing: Noo.fontSize()
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: activPal.text
          text: qsTr("Level name:")
        }
        TtextField {
          id: lName
          maximumLength: 30
          width: newLevelPopup.width - Noo.fontSize() * 12
        }
      }
      Text {
        color: activPal.text
        text: qsTr("Level description:")
      }
      TipRect {
        id: bg
        anchors.horizontalCenter: parent.horizontalCenter
        width: newLevelPopup.width - Noo.fontSize() * 2; height: Noo.fontSize() * 6.5
        border { width: lDesc.focus ? 1 : 0; color: activPal.highlight }
        color: activPal.base
        TextEdit {
          id: lDesc
          width: parent.width - Noo.fontSize(); height: Noo.fontSize() * 6
          anchors.centerIn: parent
          wrapMode: TextEdit.WordWrap
          onLengthChanged: {
            if (length > 120)
              text = text.substr(-120)
          }
        }
      }
    }

    onAccepted: creator.continueLevelSave(name, desc)
  }
}
