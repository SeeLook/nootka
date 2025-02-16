// pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: mainWin

    width: 1024
    height: 768

    FontLoader {
        id: bravuraFont

        source: "./Bravura.otf"
    }

    Repeater {
        model: 5

        Rectangle {
            x: 10
            y: 240 + (index * 20)
            width: mainWin.width - 20
            height: 1
            color: "black"
        }

    }

    Rectangle {
        anchors.fill: clef
        color: "#80aaaaaa"
        Component.onCompleted: console.log(width, height)
    }

    Text {
        id: clef

        x: 10
        y: 78
        text: "\ue062"

        font {
            family: bravuraFont.name
            pixelSize: 92
        }

    }

}
