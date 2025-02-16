/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12

Rectangle {
    id: label

    property alias score: result.text
    property color bg: activPal.button
    property int index: -1
    property bool hi: false

    signal getTip()

    function more() {
        resultsItem.more(label);
    }

    function showTip(tip) {
        NOO.setStatusTip(tip, Item.Top);
    }

    height: nootkaWindow.width / 45
    width: height * 2
    color: resultArea.containsMouse || hi ? activPal.highlight : Qt.tint(activPal.mid, NOO.alpha(bg, 40))
    z: 15

    Text {
        id: result

        font.pixelSize: parent.height * 0.75
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: resultArea.containsMouse || hi ? activPal.highlightedText : activPal.buttonText
    }

    MouseArea {
        id: resultArea

        anchors.fill: parent
        hoverEnabled: !NOO.isAndroid()
        onClicked: more()
        onContainsMouseChanged: {
            if (GLOB.showHints) {
                if (containsMouse)
                    getTip();
                else
                    NOO.setStatusTip("", Item.Top);
            }
        }
    }

}
