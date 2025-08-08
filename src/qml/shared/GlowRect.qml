/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick.Effects
import QtQuick 2.12

Rectangle {
    id: rect
    property bool rised: true
    property color shadowColor: activPal.shadow
    property real horizontalOffset: rect.rised ? NOO.factor() / 6 : 0
    property real verticalOffset: rect.rised ? NOO.factor() / 6 : 0
    // TODO: make use of it!
    property real shadowRadius: 1.0
    property real spread: 1.0
    property real cornerRadius: 1.0
    // property alias spread: glow.spread
    // property alias cornerRadius: glow.cornerRadius

    color: activPal.base
    radius: NOO.factor() / 3

    layer.enabled: true
    layer.effect: MultiEffect {
        id: glow
        shadowEnabled: true
        shadowColor: rect.shadowColor
        shadowHorizontalOffset: rect.horizontalOffset
        shadowVerticalOffset: rect.verticalOffset
        shadowScale: 1.0 + shadowHorizontalOffset / rect.width
    }

    // RectangularGlow {
    //     id: glow
    //     width: parent.width
    //     height: parent.height
    //     x: rised ? NOO.factor() / 6 : 0
    //     y: rised ? NOO.factor() / 6 : 0
    //     z: -1
    //     glowRadius: NOO.factor() / 3
    //     spread: 0
    //     color: activPal.shadow
    //     cornerRadius: parent.radius + glowRadius * spread
    // }

}
