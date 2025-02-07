/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

Text {
    id: tie

    property alias xScale: sc.xScale
    property bool stemDown: false

    text: stemDown ? "\ue204" : "\ue1fd"
    y: stemDown ? -1.25 : -0.5
    z: -20

    font {
        family: "Scorek"
        pixelSize: 7
    }

    transform: Scale {
        id: sc
    }

}
