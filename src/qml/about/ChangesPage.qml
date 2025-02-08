/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ListView {
    id: chList

    width: parent.width
    height: parent.height
    model: dialogObj.getChanges()

    delegate: Tile {
        width: chList.width
        height: relText.height + NOO.factor() * 2
        bgColor: index % 2 ? activPal.base : activPal.alternateBase

        Text {
            id: relText

            text: modelData
            x: NOO.factor() / 2
            width: chList.width - NOO.factor()
            horizontalAlignment: text.substring(0, 4) === "<h1>" ? Text.AlignHCenter : Text.AlignLeft
            color: activPal.text
            wrapMode: TextEdit.Wrap
            textFormat: Text.StyledText
        }

    }

    ScrollBar.vertical: ScrollBar {
        active: true
        visible: active
    }

}
