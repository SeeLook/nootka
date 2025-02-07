/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12

TpopupDialog {
    id: updatePopup

    rejectButton.visible: false
    width: NOO.isAndroid() ? globalPage.width - NOO.factor() : globalPage.width * 0.8
    height: NOO.isAndroid() ? globalPage.height + NOO.factor() * 3 : globalPage.height * 0.9
    bgColor: Qt.tint(activPal.window, NOO.alpha("teal", 20))
    caption: updateSum.titleText

    border {
        color: "teal"
        width: NOO.factor() / 4
    }

    TupdateSummary {
        id: updateSum

        height: parent.height + NOO.factor()
    }

}
