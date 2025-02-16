/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

Rectangle {
    property bool rised: true
    property alias shadowRadius: glow.glowRadius
    property alias shadowColor: glow.color
    property alias horizontalOffset: glow.x
    property alias verticalOffset: glow.y
    property alias spread: glow.spread
    property alias cornerRadius: glow.cornerRadius

    color: activPal.base
    radius: NOO.factor() / 3

    RectangularGlow {
        id: glow

        width: parent.width
        height: parent.height
        x: rised ? NOO.factor() / 6 : 0
        y: rised ? NOO.factor() / 6 : 0
        z: -1
        glowRadius: NOO.factor() / 3
        spread: 0
        color: activPal.shadow
        cornerRadius: parent.radius + glowRadius * spread
    }

}
