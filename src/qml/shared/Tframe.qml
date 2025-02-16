/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

/**
 * Common frame with custom background and palette-aware colors
 */
Frame {
    property alias border: bg.border
    property alias bgColor: bg.color

    background: Rectangle {
        id: bg

        color: "transparent"
        radius: NOO.factor() * 0.75

        border {
            color: enabled ? activPal.text : disdPal.text
            width: NOO.factor() * 0.1
        }

    }

}
