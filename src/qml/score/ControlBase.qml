/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Window 2.12

GlowRect {
    id: base

    property bool active: false
    property real factor: Screen.pixelDensity * (NOO.isAndroid() ? 2.5 : 3.5)
    property alias component: loader.sourceComponent
    property alias hideTimer: hideTimer
    property alias loader: loader
    // protected
    property bool show: false

    z: 20
    width: loader.item ? loader.item.width : 0
    height: loader.item ? loader.item.height : 0
    color: activPal.alternateBase
    onActiveChanged: {
        if (active) {
            loader.active = true;
            show = true;
            hideTimer.stop();
        } else {
            hideTimer.restart();
        }
    }

    Loader {
        id: loader

        active: false
    }

    Timer {
        id: hideTimer

        interval: 1000
        repeat: false
        onTriggered: show = false
    }

}
