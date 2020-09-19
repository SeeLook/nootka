/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"
import "../settings"


TtunerDialogItem {
  id: tunerDialog
  width: parent.width; height: parent.height

  Tile {
    id: exTile
    width: exCol.width + Noo.fontSize() * 2
    height: parent.height - Noo.fontSize()
    anchors { horizontalCenter: undefined; verticalCenter: parent.verticalCenter }
    description: qsTranslate("SoundPage", "Frequency of detected note. You can use this for tuning.")
    Column {
      id: exCol
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize() * (Noo.isAndroid ? 1 : 1.5)
      Repeater {
        model: tuningModel
        Row {
          x: Noo.fontSize() / 2
          property var splitText: modelData.split(";")
          Text {
            visible: GLOB.instrument.isGuitar
            anchors.verticalCenter: parent.verticalCenter
            font { family: "Nootka";  pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 1.8 : 2.2) }
            color: activPal.text; textFormat: Text.Normal
            text: index + 1
          }
          Item {
            width: childrenRect.width; height: Noo.fontSize() * (Noo.isAndroid() ? 1.3 : 1.5)
            anchors.verticalCenter: parent.verticalCenter
            Text {
              y: -height * 0.33
              font { family: "Scorek";  pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 1.6 : 2) }
              color: activPal.text; textFormat: Text.StyledText
              text: splitText[0]
            }
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            font { family: "Nootka"; pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 1 : 1.2) }
            color: activPal.text; textFormat: Text.Normal
            text: "="
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            font { pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 1.1 : 1.4) }
            color: activPal.text; textFormat: Text.Normal
            text: splitText[1]
          }
        }
      }
    }
  }

//   Text {
//     color: Qt.tint(activPal.window, Noo.alpha(activPal.highlight, 50))
//     font { family: "Nootka"; pixelSize: parent.height }
//     anchors.centerIn: parent
//     text: GLOB.instrument.glyph
//   }

  Column {
    y: spacing
    width: (tunerDialog.width - exTile.width) - Noo.fontSize()
    x: exTile.width + Noo.fontSize() / 2
    spacing: (parent.height - freqTile.height - (volRow.visible ? volRow.height : 0) - intoBar.height - freqTexts.height - inputCol.height) / 6

    MiddleA440 {
      id: freqTile
      showDesc: !Noo.isAndroid()
      anchors.horizontalCenter: parent.horizontalCenter
      width: (tunerDialog.width - exTile.width) - Noo.fontSize() * (Noo.isAndroid() ? 1 : 10)
      value: GLOB.midAfreq
      onValueChanged: workFreq = value
    }

    Row {
      id: volRow
      anchors.horizontalCenter: parent.horizontalCenter
      visible: Noo.isAndroid()
      spacing: Noo.fontSize()
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("output volume") + ":"
        color: activPal.text; font.pixelSize: Noo.fontSize()
      }
      Tslider {
        id: volSlider
        width: (tunerDialog.width - exTile.width) * 0.5
        to: Noo.isAndroid() ? SOUND.maxVolRange() : 100
        value: Noo.isAndroid() ? SOUND.currentVol() : 50
        onValueChanged: {
          if (Noo.isAndroid())
            SOUND.setVol(value)
        }
      }
    }

    Tumbler {
        id: pitchTumb
        width: parent.width
        height: Noo.fontSize() * 4
        visibleItemCount: ((width / (Noo.fontSize() * 5)) / 2) * 2 - 1
        model: highestNote() - lowestNote()
        currentIndex: pitch - lowestNote()
        delegate: Component {
          Column {
            spacing: Noo.fontSize() / 4
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            Rectangle {
              anchors.horizontalCenter: parent.horizontalCenter
              width: Math.max(childrenRect.width, childrenRect.height / 2.3); height: width
              radius: height / 2
              color: "transparent"
              border { width: isOpenString(lowestNote() + modelData) ? 2 : 0; color: activPal.text }
              Text {
                text: styledName(lowestNote() + modelData)
                color: activPal.text
                y: height * -0.2; x: (Math.max(width, height / 2.3) - width) / 2
                font { family: "Scorek"; pixelSize: Noo.fontSize() * 1.5 }
              }
            }
          }
        }
        contentItem: PathView {
          id: pathView
          model: pitchTumb.model
          delegate: pitchTumb.delegate
//           clip: true
          pathItemCount: pitchTumb.visibleItemCount + 1
          preferredHighlightBegin: 0.5
          preferredHighlightEnd: 0.5
          dragMargin: width / 2
          path: Path {
            startX: 0
            startY: Noo.fontSize() * 1.4
            PathLine {
              x: pathView.width
              y: Noo.fontSize() * 1.4
            }
          }
        }
      }

    IntonationBar {
      id: intoBar
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width * (Noo.isAndroid() ? 0.98 : 0.89); height: Noo.fontSize() * (Noo.isAndroid() ? 2 : 3)
      deviation: tunerDialog.deviation
    }

    Row {
      id: freqTexts
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize()
      TipRect {
        width: Noo.fontSize() * 5; height: Noo.fontSize() * 3; radius: height / 8; color: activPal.base
        Text {
          y: height * -0.24; x: (parent.width - width) / 2
          font { family: "Scorek"; pixelSize: Noo.fontSize() * 2 }
          text: noteName
          color: activPal.text; textFormat: Text.StyledText
        }
      }
      TipRect {
        width: Noo.fontSize() * 12; height: Noo.fontSize() * 3; radius: height / 8; color: activPal.base
        Text {
          anchors.centerIn: parent
          font { pixelSize: Noo.fontSize() * 2; bold: true }
          text: frequency; color: activPal.text
        }
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        font { pixelSize: Noo.fontSize() * 2 }
        text: Noo.TR("SoundPage", "[Hz]")
        color: activPal.text
      }
    }

    Column {
      id: inputCol
      anchors.horizontalCenter: parent.horizontalCenter
      visible: Noo.isAndroid()
      spacing: Noo.fontSize() / 2
      VolumeBar {
        id: volBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: (tunerDialog.width - exTile.width) * 0.9
        height: Noo.fontSize() * 2
        Timer {
          repeat: true; interval: 75; running: inputCol.visible && SOUND.listening
          onTriggered: volBar.volume = SOUND.inputVol()
        }
      }
      Text {
        id: minText
        anchors.horizontalCenter: parent.horizontalCenter
        text: Noo.TR("SoundPage", "Minimum volume of a sound to be pitch-detected")
        color: activPal.text; font.pixelSize: Noo.fontSize()
      }
    }
  }

  Component.onCompleted: {
    dialLoader.standardButtons = DialogButtonBox.Close
    dialLoader.title = qsTr("Nooter - Nootka tuner")
  }

  Connections {
    target: SOUND
    onVolumeUpPressed: {
      if (Noo.isAndroid())
        volSlider.value++
    }
    onVolumeDownPressed: {
      if (Noo.isAndroid())
        volSlider.value--
    }
  }
}
