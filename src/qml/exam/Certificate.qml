/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Exam 1.0
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

Item {
    anchors.fill: parent
    z: 300

    ShaderEffectSource {
        id: effectSource

        sourceItem: nootkaWindow.contentItem.parent
        anchors.fill: parent
        live: false
    }

    Colorize {
        anchors.fill: parent
        source: effectSource
        hue: 0 // always black, palette no matters
        saturation: 0
        lightness: 0
    }

    FastBlur {
        id: blur

        radius: 36
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
    }

    //* apply blur with a delay, otherwise it won't work
    Timer {
        id: timer

        interval: 20
        running: true
        onTriggered: {
            blur.source = effectSource;
            blur.width = Qt.binding(function() {
                return nootkaWindow.width;
            });
            blur.height = Qt.binding(function() {
                return nootkaWindow.height;
            });
        }
    }

    CertificateItem {
        id: cert

        parentHeight: parent.height * 0.98 + (parent.width - parent.width)
        x: parent.width - width - NOO.factor() / 2
        y: (parent.height - height) / 2
    }

    Tile {
        anchors.horizontalCenter: undefined
        width: parent.width * 0.48
        x: parent.width * 0.005
        y: parent.height * 0.01
        bgColor: Qt.tint(GLOB.correctColor, NOO.alpha(activPal.base, 180))
        description: qsTranslate("TnootkaCertificate", "Save this certificate to file in remembrance.")

        bgBorder {
            color: GLOB.correctColor
            width: 2
        }

        Column {
            width: parent.width

            Text {
                text: qsTranslate("TnootkaCertificate", "CONGRATULATIONS!<br>You have just passed the exam!")
                width: parent.width * 0.99
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                font {
                    pixelSize: width / 25
                    bold: true
                }

            }

            Image {
                source: NOO.pix("save")
                height: parent.width / 12
                width: height * (sourceSize.width / sourceSize.height)
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: cert.save()
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        }

    }

    Tile {
        width: parent.width * 0.48
        x: parent.width * 0.005
        bgColor: Qt.tint("blue", NOO.alpha(activPal.base, 180))

        anchors {
            horizontalCenter: undefined
            verticalCenter: parent.verticalCenter
        }

        bgBorder {
            color: "blue"
            width: 2
        }

        Column {
            width: parent.width

            Text {
                text: qsTranslate("TnootkaCertificate", "You can still play with it and improve effectiveness.")
                width: parent.width * 0.99
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Image {
                source: NOO.pix("nextQuest")
                height: parent.width / 12
                width: height * (sourceSize.width / sourceSize.height)
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: cert.continueExam()
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        }

    }

    Tile {
        anchors.horizontalCenter: undefined
        width: parent.width * 0.48
        x: parent.width * 0.005
        y: parent.height * 0.99 - height
        bgColor: Qt.tint(GLOB.wrongColor, NOO.alpha(activPal.base, 180))

        bgBorder {
            color: GLOB.wrongColor
            width: 2
        }

        Column {
            width: parent.width

            Text {
                text: qsTr("Stop exam")
                width: parent.width * 0.99
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                font {
                    pixelSize: width / 25
                    bold: true
                }

            }

            Image {
                source: NOO.pix("stopExam")
                height: parent.width / 12
                width: height * (sourceSize.width / sourceSize.height)
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: cert.stopExam()
            cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        }

    }

}
