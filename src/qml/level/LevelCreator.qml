/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
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
    pages.addItem("levelsSett", qsTr("Levels"), "level/Levels")
    pages.addItem("questionsSett", qsTr("Questions"), "level/Questions")
    pages.addItem("melodySett", qsTr("Melody"), "level/Melody")
    pages.addItem("rhythms-frame", qsTr("Rhythms"), "level/Rhythms")
    pages.addItem("accidSett", qsTr("Accidentals"),  "level/Accids")
    pages.addItem("rangeSett", qsTr("Range"),  "level/Range")
    if (Noo.isAndroid()) {
        dialLoader.buttons = [DialogButtonBox.Close, DialogButtonBox.Help, DialogButtonBox.RestoreDefaults]
    } else {
        dialLoader.title = creator.title
        dialLoader.standardButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults
        var b = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults)
        b.text = qsTr("Check")
        b.DialogButtonBox.buttonRole = DialogButtonBox.ActionRole
        b.pixmap = Noo.pix("levelCreator")
        b.clicked.connect(checkLevel)
    }
  }

  onSaveStateChanged: {
    pages.buttons[0].pixmap = Noo.pix(creator.notSaved ? "notSaved" : "levelsSett")
    dialLoader.title = creator.title // TODO: it doesn't work
  }

  onSave: {
    if (typeof(pages.pages[2]) !== "string") // save melody
      pages.pages[2].saveLevel()
  }

  Connections {
    target: pages.mobileButt
    onClicked: dialLoader.dialogDrawer.open()
  }

  function help() { Noo.openHelpLink("level-creator") }
}
