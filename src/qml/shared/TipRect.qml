/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick.Effects
import QtQuick 2.12

Item {
    id: tip

    property alias color: bg.color
    property alias radius: bg.radius
    property bool rised: true
    property real shadowRadius: 8.0 // dummy
    property alias shadowColor: shd.shadowColor
    property alias border: bg.border
    property alias horizontalOffset: shd.shadowHorizontalOffset
    property alias verticalOffset: shd.shadowVerticalOffset

    Rectangle {
        id: bg

        anchors.fill: parent
        color: activPal.base
        radius: NOO.factor() / 3
        visible: false
        clip: true
    }

    MultiEffect {
        id: shd
        source: bg
        anchors.fill: bg
        shadowEnabled: true
        shadowColor: activPal.shadow
        // blur: tip.shadowRadius / 64.0
        shadowHorizontalOffset: rised ? NOO.factor() / 5 : 0
        shadowVerticalOffset: rised ? NOO.factor() / 5 : 0
    }
}
