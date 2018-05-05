/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka.Exam 1.0
import "../"


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
    hue: 0; saturation: 0; lightness: 0 // always black, palette no matters
  }

  FastBlur {
    id: blur
    radius: 36
  }

  MouseArea { anchors.fill: parent; hoverEnabled: true }

  /** apply blur with a delay, otherwise it won't work  */
  Timer {
    id: timer
    interval: 20; running: true
    onTriggered: {
      blur.source = effectSource
      blur.width = Qt.binding( function() { return nootkaWindow.width })
      blur.height = Qt.binding( function() { return nootkaWindow.height })
    }
  }

  CertificateItem {
    id: cert
    width: parent.width / 2; height: parent.height * 0.98
    x: parent.width * 0.51; y: parent.height * 0.01
  }

  Tile {
    anchors.horizontalCenter: undefined
    width: parent.width * 0.48; x: parent.width * 0.005; y: parent.height * 0.01
    bgColor: Qt.tint(GLOB.correctColor, Noo.alpha(activPal.base, 180)); bgBorder { color: GLOB.correctColor; width: 2 }
    Column {
      width: parent.width
      Text {
        text: qsTranslate("TnootkaCertificate", "CONGRATULATIONS!<br>You have just passed the exam!")
        width: parent.width * 0.99
        font { pixelSize: width / 25; bold: true }
        horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
      }
      Image {
        source: Noo.pix("save")
        height: parent.width / 12; width: height * (sourceSize.width / sourceSize.height)
        anchors.horizontalCenter: parent.horizontalCenter
      }
    }
    description: qsTranslate("TnootkaCertificate", "Save this certificate to file in remembrance.")
    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      onClicked: cert.save()
      cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
  }

  Tile {
    anchors { horizontalCenter: undefined; verticalCenter: parent.verticalCenter }
    width: parent.width * 0.48; x: parent.width * 0.005
    bgColor: Qt.tint("blue", Noo.alpha(activPal.base, 180)); bgBorder { color: "blue"; width: 2 }
    Column {
      width: parent.width
      Text {
        text: qsTranslate("TnootkaCertificate", "You can still play with it and improve effectiveness.")
        width: parent.width * 0.99
        horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
      }
      Image {
        source: Noo.pix("nextQuest")
        height: parent.width / 12; width: height * (sourceSize.width / sourceSize.height)
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
    width: parent.width * 0.48; x: parent.width * 0.005; y: parent.height * 0.99 - height
    bgColor: Qt.tint(GLOB.wrongColor, Noo.alpha(activPal.base, 180)); bgBorder { color: GLOB.wrongColor; width: 2 }
    Column {
      width: parent.width
      Text {
        text: qsTr("Stop exam")
        width: parent.width * 0.99
        font { pixelSize: width / 25; bold: true }
        horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
      }
      Image {
        source: Noo.pix("stopExam")
        height: parent.width / 12; width: height * (sourceSize.width / sourceSize.height)
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
