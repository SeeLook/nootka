/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

pragma ComponentBehavior: Bound

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: settings

    property int clef: GLOB.clefType
    property int instrument: GLOB.instrument.typeINT

    width: parent.width
    height: parent.height

    function apply() {
        GLOB.clefType = clef; // it can be changed either by score or instrument page
        if (NOO.isAndroid()) {
            // it may occur only when rotation was enabled and user changed to saxophone

            if (NOO.instr(instrument).isSax)
                GLOB.setDisableRotation(true);
 // but has not opened phone settings
        }
        for (var i = 0; i < pages.pages.length; ++i) {
            if (typeof (pages.pages[i]) === 'object')
                pages.pages[i].save();

        }
    }

    function reset() {
        pages.currentPage.defaults();
    }

    function help() {
        pages.currentPage.help();
    }

    onInstrumentChanged: {
        if (pages.buttons.length > 2)
            pages.buttons[2].pixmap = NOO.pix("pane/i-0" + instrument);
    }

    Component.onCompleted: {
        if (!GLOB.isExam) {
            pages.addItem("global", qsTr("Common"), "settings/Global");
            pages.addItem("score", qsTr("Score"), "settings/Score");
            pages.addItem("i-0" + GLOB.instrument.typeINT, qsTr("Instrument"), "settings/Instrument");
            pages.addItem("sound", qsTr("Sound"), "settings/Sound");
        }
        pages.addItem("questions", qsTr("Exercises") + "\n& " + qsTr("Exam"), "settings/Exam");
        if (NOO.isAndroid() && !GLOB.isExam)
            pages.addItem("phoneSett", qsTr("Phone") + "\n& " + qsTr("Tablet"), "settings/Phone");

        dialLoader.stdButtons = DialogButtonBox.Apply | DialogButtonBox.Cancel | DialogButtonBox.RestoreDefaults | DialogButtonBox.Help;
        dialLoader.title = "Nootka - " + qsTranslate("TsettingsDialog", "application's settings");
    }

    PagesDialog {
        id: pages
    }

}
