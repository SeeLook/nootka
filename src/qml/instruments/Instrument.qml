/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import "score"

Flickable {
    id: instrFlick

    property var instrument: null
    property var score: null
    property real hiFactor: GLOB.instrument.getItemHeight(100) / 100

    function setInstrument() {
        if (instrument) {
            score.parent.scale = 1;
            instrument.destroy();
        }
        if (GLOB.instrument.type)
            instrument = Qt.createComponent("qrc:/instruments/" + GLOB.instrument.qmlFile + ".qml").createObject(sizable);
        else
            instrument = null;
        if (GLOB.instrument.piano)
            instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote());

        NOO.instrument = instrument;
        if (instrument && !GLOB.instrument.isSax)
            score.parent.scale = Qt.binding(function() {
            return (1 - hiFactor * instrument.scale) / (1 - hiFactor);
        });

    }

    boundsBehavior: Flickable.StopAtBounds
    height: sizable.height
    width: nootkaWindow.width * (GLOB.instrument.isSax ? 0.15 : 1)
    y: GLOB.instrument.isSax ? 0 : nootkaWindow.contentItem.height - height
    x: GLOB.instrument.isSax ? parent.width - width : 0
    z: instrument && instrument.scale > 1 ? 7 : (GLOB.instrument.isSax ? 6 : 1)
    contentWidth: sizable.width
    contentHeight: sizable.height
    Component.onCompleted: setInstrument()

    Item {
        id: sizable

        width: instrument ? instrument.width * instrument.scale : 0
        height: instrument ? instrument.height * instrument.scale : 0
    }

    Connections {
        target: GLOB
        onInstrumentChanged: setInstrument()
    }

    Connections {
        target: score
        enabled: GLOB.instrument.piano
        onClefChanged: instrument.setAmbitus(score.scoreObj.lowestNote(), score.scoreObj.highestNote())
    }

}
