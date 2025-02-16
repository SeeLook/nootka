/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../level"
import QtQuick 2.12

Tmenu {
    id: lPop

    property alias levelPreview: levelPreview

    width: NOO.factor() * 40
    height: parent.height / 2
    x: (parent.width - width) / 2
    y: parent.height / 12
    visible: true
    onClosed: destroy()

    Column {
        width: parent.width
        padding: NOO.factor() / 2

        LevelPreview {
            id: levelPreview

            width: parent.width - NOO.factor()
            height: lPop.height - NOO.factor()
        }

    }

}
