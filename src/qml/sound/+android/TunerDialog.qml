/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka 1.0
import Nootka.Dialogs 1.0
import "../"
import "../settings"


TtunerDialogItem {
  id: tunerDialog
  width: parent.width; height: parent.height

  property bool widthEnough: width > NOO.factor() * 45

  Drawer {
    id: exDrawer
    width: exCol.width + NOO.factor() * 2
    height: parent.height
    visible: widthEnough; modal: false
    closePolicy: widthEnough ? Popup.NoAutoClose : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: TipRect { color: activPal.base; radius: 0 }

    Column {
      id: exCol
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: NOO.factor() / 2
      Text {
        width: parent.width
        color: activPal.text
        text: qsTranslate("SoundPage", "Frequency of detected note. You can use this for tuning.")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: NOO.factor() * 0.8
        wrapMode: Text.WordWrap
      }
      Item { width: NOO.factor(); height: NOO.factor() / 4 }
      Repeater {
        model: tuningModel
        Row {
          property var splitText: modelData.split(";")
          Text {
            visible: GLOB.instrument.isGuitar
            anchors.verticalCenter: parent.verticalCenter
            font { family: "Nootka";  pixelSize: NOO.factor() * 1.8 }
            color: activPal.text; textFormat: Text.Normal
            text: index + 1
          }
          Item {
            width: childrenRect.width; height: NOO.factor() * 1.3
            anchors.verticalCenter: parent.verticalCenter
            Text {
              y: -height * 0.33
              font { family: "Scorek";  pixelSize: NOO.factor() * 1.6 }
              color: activPal.text; textFormat: Text.StyledText
              text: splitText[0]
            }
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            font { family: "Nootka"; pixelSize: NOO.factor() }
            color: activPal.text; textFormat: Text.Normal
            text: "="
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            font { pixelSize: NOO.factor() * 1.1 }
            color: activPal.text; textFormat: Text.Normal
            text: splitText[1]
          }
        }
      }
      Item { width: NOO.factor(); height: NOO.factor() / 4 }
      TcuteButton {
        width: parent.width
        text: "A = " + workFreq + " " + NOO.TR("SoundPage", "[Hz]")
        onClicked: midApopup.open()
      }
    }
  }

  Column {
    id: tunCol
    width: tunerDialog.width - (widthEnough ? exDrawer.width : 0) - NOO.factor()
    x: widthEnough ? exDrawer.width + NOO.factor() / 2 : 0
    anchors.verticalCenter: parent.verticalCenter
    spacing: Math.min(NOO.factor() * 3, (parent.height - volRow.height - pitchCol.height - inputCol.height) / 4)

    Row {
      id: volRow
      x: widthEnough ? NOO.factor() : (parent.width - width) / 2
      spacing: NOO.factor()
      Text {
        id: outVolText
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("output volume").replace(" ", "\n") + ":"
        color: activPal.text
      }
      Tslider {
        id: volSlider
        width: tunCol.width - outVolText.width - NOO.factor() * 10
        anchors.verticalCenter: parent.verticalCenter
        to: SOUND.maxVolRange()
        value: SOUND.currentVol()
        onValueChanged: SOUND.setVol(value)
      }
    }

    Column {
      id: pitchCol
      spacing: NOO.factor()
      width: parent.width

      Tumbler {
        id: pitchTumb
        width: parent.width; height: NOO.factor() * 4
        anchors.horizontalCenter: parent.horizontalCenter
        visibleItemCount: ((width / (NOO.factor() * 5)) / 2) * 2 - 1
        model: highestNote() - lowestNote()
        currentIndex: pitch - lowestNote()
        wrap: false
        delegate: Component {
          Column {
            spacing: NOO.factor() / 4
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            Item {
              property int strNr: GLOB.instrument.isGuitar ? whichString(lowestNote() + modelData) : 0
              anchors.horizontalCenter: parent.horizontalCenter
              Text {
                visible: parent.strNr > 0
                x: strNameTxt.x + (strNameTxt.width - width) / 2
                text: parent.strNr > 0 ? parent.strNr : ""
                font { family: "Nootka"; pixelSize: NOO.factor() * 1.5 }
                property real dev: Math.abs(deviation)
                color: pitchTumb.currentIndex === index ? (dev > 0 ? (dev > 0.3 ? "red" : (dev > 0.1 ? "yellow" : "lime")) : activPal.text) : activPal.text
                Behavior on color { ColorAnimation { duration: 150 }}
                y: pitchTumb.currentIndex === index ? 0 : -height / 2
                transformOrigin: Item.Top
                scale: pitchTumb.currentIndex === index ? 1.6 : 1
              }
              Text {
                id: strNameTxt
                visible: !GLOB.instrument.isGuitar || pitchTumb.currentIndex !== index
                text: styledName(lowestNote() + modelData)
                color: activPal.text
                y: height * -0.2; x: -width / 2
                font { family: "Scorek"; pixelSize: NOO.factor() * 1.5 }
              }
            }
          }
        }
        contentItem: PathView {
          id: pathView
          interactive: false
          model: pitchTumb.model
          delegate: pitchTumb.delegate
          pathItemCount: pitchTumb.visibleItemCount + 1
          preferredHighlightBegin: 0.5
          preferredHighlightEnd: 0.5
          path: Path {
            startX: 0
            startY: NOO.factor() * 1.4
            PathLine {
              x: pathView.width
              y: NOO.factor() * 1.4
            }
          }
        }
      }

      IntonationBar {
        id: intoBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: tunCol.width * 0.98; height: NOO.factor() * 3
        deviation: tunerDialog.deviation
        pitchText: noteName
      }

      Row {
        id: freqTexts
        x: (parent.width - width) / 2 + NOO.factor() * 2.5 // keep bg rect in the middle
        spacing: NOO.factor()
        TipRect {
          width: NOO.factor() * 12; height: NOO.factor() * 3; radius: height / 8; color: activPal.base
          Text {
            anchors.centerIn: parent
            font { pixelSize: NOO.factor() * 2; bold: true }
            text: frequency; color: activPal.text
          }
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          font { pixelSize: NOO.factor() * 2 }
          text: NOO.TR("SoundPage", "[Hz]")
          color: activPal.text
        }
      }
    }

    Column {
      id: inputCol
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: NOO.factor() / 2
      VolumeBar {
        id: volBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: tunCol.width * 0.9; height: NOO.factor() * 2
        Timer {
          repeat: true; interval: 75; running: inputCol.visible && SOUND.listening
          onTriggered: volBar.volume = SOUND.inputVol()
        }
      }
      Text {
        id: minText
        anchors.horizontalCenter: parent.horizontalCenter
        width: tunCol.width - NOO.factor(); horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: NOO.TR("SoundPage", "Minimum volume of a sound to be pitch-detected")
        color: activPal.text
      }
    }
  }

  TcuteButton {
    id: drButt
    x: NOO.factor() / 2; y: NOO.factor() / 2
    height: NOO.factor() * 3
    visible: !exDrawer.visible
    font { pixelSize: NOO.factor() * 2; bold: true }
    text: "⋮"
    color: Qt.tint(activPal.window, NOO.alpha("green", 20))
    onClicked: exDrawer.open()
  }

  TiconButton {
    x: tunerDialog.width - width - NOO.factor() / 2; y: NOO.factor() / 2
    pixmap: NOO.pix("exit")
    iconHeight: drButt.height
    color: Qt.tint(activPal.window, NOO.alpha("red", 20))
    onClicked: dialLoader.close()
  }

  TpopupDialog {
    id: midApopup
    width: tunerDialog.width - NOO.factor() * 2; height: freqTile.height + NOO.factor() * 4
    y: tunerDialog.height - height - NOO.factor()

    border { color: activPal.highlight; width: NOO.factor() / 4.0 }
    acceptButton.visible: false
    rejectButton.text: NOO.TR("QShortcut", "Close")

    MiddleA440 {
      id: freqTile
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width - NOO.factor()
      value: GLOB.midAfreq
      onValueChanged: workFreq = value
    }
  }

  onWantClose: dialLoader.close()

  Component.onCompleted: dialLoader.stdButtons = DialogButtonBox.NoButton

  Connections {
    target: SOUND
    onVolumeUpPressed: volSlider.value++
    onVolumeDownPressed: volSlider.value--
  }
}
