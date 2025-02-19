/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Dialogs 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TlevelCreatorItem {
    id: creator

    function help() {
        NOO.openDocLink("2017/05/17/creating-levels/");
    }

    function reset() {
    }

    width: parent.width
    height: parent.height
    Component.onCompleted: {
        pages.addItem("levels", qsTr("Levels"), "level/Levels");
        pages.addItem("questionNote", qsTr("Questions"), "level/Questions");
        pages.addItem("melody", qsTr("Melody"), "level/Melody");
        pages.addItem("rhythms", qsTr("Rhythms"), "level/Rhythms");
        pages.addItem("accidentals", qsTr("Accidentals"), "level/Accids");
        pages.addItem("range", qsTr("Range"), "level/Range");
        dialLoader.title = title;
        dialLoader.stdButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults;
        var b = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults);
        b.text = qsTr("Check");
        b.pixmap = NOO.pix("levelCreator");
        b.clicked.connect(checkLevel);
    }
    onSaveStateChanged: {
        pages.buttons[0].pixmap = NOO.pix(notSaved ? "pane/notSaved" : "pane/levels");
        dialLoader.title = title;
    }
    onUpdateLevel: {
        pages.buttons[1].pixmap = NOO.pix("pane/question" + (isMelody ? "Melody" : "Note"));
        pages.buttons[2].enabled = isMelody; // melody button
        pages.buttons[3].enabled = isMelody && howGetMelody != 2; // rhythms button (melody but not from the list)
        pages.buttons[5].enabled = !isMelody || (isMelody && howGetMelody == 0); // range button
    }
    onSave: {
        // save melody

        if (typeof (pages.pages[2]) !== "string")
            pages.pages[2].saveLevel();

    }
    onWantValidationMessage: title, message, accent => {
        var c = Qt.createComponent("qrc:/level/LevelValidationMessage.qml");
        c.createObject(creator, {
            "caption": title,
            "message": message,
            "accent": accent
        });
    }

    PagesDialog {
        id: pages
    }

}
