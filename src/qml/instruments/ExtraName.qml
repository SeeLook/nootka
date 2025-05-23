/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

DropShadow {
    property alias text: eText.text
    property alias fSize: eText.font.pixelSize

    z: 30
    width: eText.width
    height: eText.height
    horizontalOffset: fSize / 25
    verticalOffset: fSize / 25
    color: activPal.shadow
    radius: NOO.factor() / 3
    source: eText
    visible: text !== ""

    Text {
        id: eText

        visible: false
        style: Text.Outline
        styleColor: activPal.shadow

        font {
            family: "Scorek"
            pixelSize: NOO.factor() * 5
        }

    }

}
