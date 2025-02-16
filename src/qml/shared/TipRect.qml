/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

Item {
    property alias color: bg.color
    property alias radius: bg.radius
    property bool rised: true
    property alias shadowRadius: shadow.radius
    property alias shadowColor: shadow.color
    property alias border: bg.border
    property alias horizontalOffset: shadow.horizontalOffset
    property alias verticalOffset: shadow.verticalOffset

    Rectangle {
        id: bg

        anchors.fill: parent
        color: activPal.base
        radius: NOO.factor() / 3
        visible: false
        clip: true
    }

    DropShadow {
        id: shadow

        anchors.fill: bg
        horizontalOffset: rised ? NOO.factor() / 5 : 0
        verticalOffset: rised ? NOO.factor() / 5 : 0
        radius: 8
        samples: 1 + radius * 2
        color: activPal.shadow
        source: bg
    }

}
