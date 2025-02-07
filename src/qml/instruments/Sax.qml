/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TsaxBg {
    //   Rectangle { // debug usage only
    //     id: line
    //     width: 2; height: parent.height
    //     x: parent.height * 0.096
    //     color: "blue"
    //   }
    //TextEdit { // for debug purposes - number representing fingers shape
    //anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter
    //font.pixelSize: 30
    //text: fingeringId
    //}

    id: instrItem

    // private
    property real xAxis: height * 0.096
    // line.x
    property color checkedColor: activPal.text
    property var activeFlap: null

    width: Math.max(nootkaWindow.width * 0.15, nootkaWindow.height * 0.21)
    height: nootkaWindow.height
    onWantNoteName: {
        if (!extraName) {
            extraName = Qt.createComponent("qrc:/instruments/ExtraName.qml").createObject(instrItem, {
                "y": instrItem.height * 0.22
            });
            extraName.fSize = Qt.binding(function() {
                return instrItem.height / 18;
            });
            extraName.x = Qt.binding(function() {
                return width - extraName.width - 2 * NOO.factor();
            });
        }
        extraName.text = name;
    }
    onCorrectInstrument: {
        var correctAnim = animComp.createObject(instrItem);
        correctAnim.start();
    }

    // body
    Image {
        parent: nootkaWindow.contentItem
        cache: false
        source: NOO.pix("saxAlto")
        height: instrFlick.height * 0.9
        width: height * (sourceSize.width / sourceSize.height)
        x: parent.width * 0.85 - instrFlick.contentX - width * 0.25
    }

    // left fingers
    SaxFlap {
        nr: 0
        width: parent.height / 34
        x: xAxis - width
        y: parent.height * 0.1
    }

    SaxFlap {
        nr: 1
        x: xAxis - width / 2
        y: parent.height * 0.14
    }

    SaxFlap {
        nr: 2
        width: parent.height / 34
        x: xAxis
        y: parent.height * 0.2
    }

    SaxFlap {
        nr: 3
        x: xAxis - width / 2
        y: parent.height * 0.25
    }

    SaxFlap {
        nr: 4
        x: xAxis - width / 2
        y: parent.height * 0.32
    }
    // left side flaps

    SaxFlap {
        nr: 5
        x: xAxis - width / 2
        y: parent.height * 0.5
    }

    SaxFlap {
        nr: 6
        x: xAxis - width / 2
        y: parent.height * 0.56
    }

    SaxFlap {
        nr: 7
        x: xAxis - width / 2
        y: parent.height * 0.62
    }
    // left pinkie cluster

    SaxFlap {
        nr: 8
        x: xAxis + width * 1.5
        y: parent.height * 0.17
        width: parent.height / 34
        height: width * 2
        rotation: -10
    }

    SaxFlap {
        nr: 9
        x: xAxis + width * 2.25
        y: parent.height * 0.13
        width: parent.height / 34
        height: width * 2
        rotation: -10
    }

    SaxFlap {
        nr: 10
        x: xAxis + width * 3.25
        y: parent.height * 0.17
        width: parent.height / 34
        height: width * 2
        rotation: -10
    }
    // left thumb

    SaxFlap {
        nr: 11
        x: xAxis - width * 2
        width: parent.height / 22
        height: width * 0.8
        y: parent.height * 0.22
    }

    // right hand
    SaxFlap {
        nr: 12
        x: xAxis + width
        y: parent.height * 0.4
        width: parent.height / 18
        height: width / 2
    }

    SaxFlap {
        nr: 13
        x: xAxis + width * 1.2
        y: parent.height * 0.43
        width: parent.height / 26
        height: width / 2
    }

    SaxFlap {
        nr: 14
        x: xAxis + width * 2.2
        y: parent.height * 0.43
        width: parent.height / 26
        height: width / 2
    }

    SaxFlap {
        nr: 15
        x: xAxis + width
        y: parent.height * 0.45
        width: parent.height / 18
        height: width / 2
    }
    // right hand side flaps

    SaxFlap {
        nr: 16
        x: xAxis - width * 2.5
        y: parent.height * 0.4
        width: parent.height / 34
        height: width * 2
        radius: 0
    }

    SaxFlap {
        nr: 17
        x: xAxis - width * 2.5
        y: parent.height * 0.46
        width: parent.height / 34
        height: width * 1.8
        radius: 0
    }

    SaxFlap {
        nr: 18
        x: xAxis - width * 2.5
        y: parent.height * 0.515
        width: parent.height / 34
        height: width * 1.5
        radius: 0
    }
    // ...

    SaxFlap {
        nr: 19
        x: xAxis - width * 2.5
        y: parent.height * 0.6
        width: parent.height / 34
        height: width * 2
        radius: 0
        rotation: 10
    }

    SaxFlap {
        nr: 20
        x: xAxis - width * 2.6
        y: parent.height * 0.66
        width: parent.height / 34
        height: width * 2
        radius: 0
        rotation: 10
    }
    // right pinkie flaps

    SaxFlap {
        nr: 21
        x: xAxis - width * 1.5
        y: parent.height * 0.75
        width: parent.height / 18
        height: width / 2
        rotation: 30
    }

    SaxFlap {
        nr: 22
        x: xAxis - width * 1.5
        y: parent.height * 0.78
        width: parent.height / 18
        height: width / 2
        rotation: 30
    }

    OutScaleTip {
        show: !active && outOfScale
        width: parent.width * 0.9
    }

    // sax flap cursor
    Rectangle {
        parent: activeFlap
        anchors.fill: parent ? parent : undefined
        z: 2
        color: GLOB.fingerColor
        radius: parent ? parent.radius : 0
        visible: active
        scale: parent && parent.pressed ? 0.3 : 1

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    Component {
        id: animComp

        SequentialAnimation {
            onStopped: destroy()

            ColorAnimation {
                target: instrItem
                property: "checkedColor"
                to: activPal.base
                duration: 400
            }

            ScriptAction {
                script: instrItem.applyCorrect()
            }

            ColorAnimation {
                target: instrItem
                property: "checkedColor"
                to: activPal.text
                duration: 400
            }

            ScriptAction {
                script: instrItem.finishCorrectAnim()
            }

        }

    }

}
