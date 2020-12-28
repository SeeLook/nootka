/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0


TlevelCreatorItem {
  id: creator

  width: parent.width
  height: parent.height

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("levels", qsTr("Levels"), "level/Levels")
    pages.addItem("questionNote", qsTr("Questions"), "level/Questions")
    pages.addItem("melody", qsTr("Melody"), "level/Melody")
    pages.addItem("rhythms", qsTr("Rhythms"), "level/Rhythms")
    pages.addItem("accidentals", qsTr("Accidentals"), "level/Accids")
    pages.addItem("range", qsTr("Range"), "level/Range")

    dialLoader.title = title
    dialLoader.standardButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults
    var b = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults)
    b.text = qsTr("Check")
    b.DialogButtonBox.buttonRole = DialogButtonBox.ActionRole
    b.pixmap = Noo.pix("levelCreator")
    b.clicked.connect(checkLevel)
  }

  onSaveStateChanged: {
    pages.buttons[0].pixmap = Noo.pix(notSaved ? "pane/notSaved" : "pane/levels")
    dialLoader.title = title
  }

  onUpdateLevel: {
    pages.buttons[1].pixmap = Noo.pix("pane/question" + (isMelody ? "Melody" : "Note"))
    pages.buttons[2].enabled = isMelody // melody button
    pages.buttons[3].enabled = isMelody && randMelody != 2 // rhythms button (melody but not from the list)
    pages.buttons[5].enabled = !isMelody || (isMelody && randMelody == 0) // range button
  }

  onSave: {
    if (typeof(pages.pages[2]) !== "string") // save melody
      pages.pages[2].saveLevel()
  }

  onWantValidationMessage: {
    var c = Qt.createComponent("qrc:/level/LevelValidationMessage.qml")
    c.createObject(creator, { "caption": title, "message": message, "accent": accent })
  }

  function help() { Noo.openHelpLink("level-creator") }
}
