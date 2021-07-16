/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


Column {
  id: levelsPage

  height: parent.height; width: parent.width

  spacing: levelsPage.width / 50
  LevelsSelector {
    id: selector
    width: parent.width
    height: levelsPage.height - buttRow.height - levelsPage.width / 25 - (NOO.isAndroid() ? 0 : NOO.factor() * 3)
    Component.onCompleted: creator.selector = selector
  }
  Row {
    id: buttRow
    spacing: levelsPage.width / 50
    anchors.horizontalCenter: parent.horizontalCenter
    TiconButton {
      enabled: creator.notSaved
      width: levelsPage.width / 4
      pixmap: NOO.pix("save"); iconHeight: levelsPage.height / 15
      text: NOO.TR("QShortcut", "Save")
      onClicked: creator.saveLevel()
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: NOO.pix("exam"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exam")
      onClicked: start(2, selector.currentLevelVar())
    }
    TiconButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      pixmap: NOO.pix("practice"); iconHeight: levelsPage.height / 15
      text: qsTr("Start exercise")
      onClicked: start(4, selector.currentLevelVar())
    }
  }
  LinkText {
    visible: !NOO.isAndroid()
    text: qsTr("Get more levels <a href=\"%1\">from Nootka home page</a>").arg("https://nootka.sourceforge.io/index.php/download/#levels")
    anchors.horizontalCenter: parent.horizontalCenter
  }

  function start(action, argument) {
    GLOB.isExam = true
    nootkaWindow.executor.init(action, argument)
    dialLoader.close()
  }

  property var notSavedMess: null
  Connections {
    target: creator
    onSaveNewLevel: {
      newLevelPopup.name = name === "" ? qsTr("new level") : name
      newLevelPopup.desc = desc
      newLevelPopup.open()
    }
    onWantNotSavedMessage: {
      if (!notSavedMess) {
        notSavedMess = Qt.createComponent("qrc:/Tmessage.qml").createObject(creator, { "accent": "#ff557f" })
        notSavedMess.rejectButton.visible = true
        notSavedMess.acceptButton.text = NOO.TR("QShortcut", "Save")
        notSavedMess.rejectButton.text = NOO.TR("QPlatformTheme", "Discard")
        notSavedMess.acceptButton.pixmap = NOO.pix("pane/notSaved")
        notSavedMess.accepted.connect(creator.saveLevel)
        notSavedMess.rejected.connect(creator.resumeAfterLevelChange) // discard button clicked
        notSavedMess.discarded.connect(creator.resumeAfterLevelChange) // when clicked out of popup
      }
      notSavedMess.caption = title
      notSavedMess.message = message
      notSavedMess.open()
    }
  }

  TpopupDialog {
    id: newLevelPopup
    property alias name: lName.text
    property alias desc: lDesc.text

    modal: true
    width: popCol.width + NOO.factor() * 2; height: popCol.height + NOO.factor() * 5
    x: (parent.width - width) / 2; y: NOO.isAndroid() ? NOO.factor() : (parent.height - height) / 2
    bgColor: Qt.tint(activPal.window, NOO.alpha(activPal.highlight, 30))
    border { color: activPal.highlight; width: NOO.factor() / 4.0 }
    glowRect.radius: NOO.factor()

    rejectButton.text: NOO.TR("QPlatformTheme", "Discard")
    acceptButton.text: NOO.TR("QShortcut", "Save")
    acceptButton.pixmap: NOO.pix("pane/notSaved")

    Column {
      id: popCol
      anchors.centerIn: parent
      spacing: NOO.factor()
      Row {
        anchors { right: parent.right; rightMargin: NOO.factor() / 2 }
        spacing: NOO.factor()
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: activPal.text
          text: qsTr("Level name:")
        }
        TtextField {
          id: lName
          maximumLength: 36
          width: NOO.factor() * 27
        }
      }
      Text {
        color: activPal.text
        text: qsTr("Level description:")
      }
      GlowRect {
        id: bg
        anchors.horizontalCenter: parent.horizontalCenter
        width: newLevelPopup.width - NOO.factor() * 2; height: NOO.factor() * 6.5
        border { width: lDesc.focus ? 1 : 0; color: activPal.highlight }
        color: activPal.base
        TextEdit {
          id: lDesc
          width: parent.width - NOO.factor(); height: NOO.factor() * 6
          anchors.centerIn: parent
          selectByMouse: true
          selectedTextColor: activPal.highlightedText
          selectionColor: activPal.highlight
          wrapMode: TextEdit.WordWrap
          onLengthChanged: {
            if (length > 255)
              text = text.substr(-255)
          }
        }
      }
    }

    onAccepted: creator.continueLevelSave(name, desc)
    onRejected: creator.resumeAfterLevelChange()
  }
}
