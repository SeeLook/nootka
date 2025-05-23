/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    property alias toolBar: toolBar
    property var scoreMenu: null
    property var melodyMenu: null

    function open() {
        if (!scoreMenu)
            scoreMenu = Qt.createComponent("qrc:/ScoreMenuContent.qml").createObject(nootkaWindow.contentItem);

        scoreMenu.open();
    }

    function melodyOpen() {
        if (!melodyMenu)
            melodyMenu = Qt.createComponent("qrc:/MelodyMenuContent.qml").createObject(nootkaWindow.contentItem);

        melodyMenu.open();
    }

    TtoolBar {
        id: toolBar
    }

}
