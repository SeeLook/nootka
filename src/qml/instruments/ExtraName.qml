/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick.Effects
import QtQuick 2.12

MultiEffect {
    property alias text: eText.text
    property alias fSize: eText.font.pixelSize

    z: 30
    width: eText.width
    height: eText.height
    shadowEnabled: true
    shadowHorizontalOffset: fSize / 40
    shadowVerticalOffset: fSize / 40
    shadowColor: activPal.shadow
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
