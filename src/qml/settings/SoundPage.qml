/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9 
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "../"


Column {
  spacing: Noo.fontSize()
  width: parent.width; height: parent.height
  topPadding: Noo.fontSize() / 2

  ListView {
    id: headList
    orientation: ListView.Horizontal
    spacing: Noo.fontSize()
    width: parent.width

    model: ListModel {
      ListElement { head: QT_TR_NOOP("listening") }
      ListElement { head: QT_TR_NOOP("playing") }
    }

    delegate: TcuteButton {
      text: qsTranslate("TscoreSettings", head)
      onClicked: { stack.currentIndex = index; headList.currentIndex = index }
      checked: index === stack.currentIndex
      Component.onCompleted: headList.height = Math.max(height, headList.height)
    }
  }

  StackLayout {
    id: stack
    height: parent.height - headList.height - Noo.fontSize() * 1.5
    width: parent.width
    currentIndex: -1

    Tflickable { // "listening" page
      contentHeight: inCol.height
      contentWidth: width

      Column {
        id: inCol
        width: parent.width

        TcheckBox {
          id: enableInChB
          text: qsTr("enable pitch detection")
          anchors.horizontalCenter: parent.horizontalCenter
          checked: true
        }

        Column {
          enabled: enableInChB.checked
          width: parent.width
          spacing: Noo.isAndroid() ? 2 : Noo.fontSize() / 2

          Tile {
            description: qsTr("Be sure your input device (microphone, webcam, instrument, etc.) is plugged in, properly configured, and working.")
            Row {
              spacing: Noo.fontSize()
              anchors.horizontalCenter: parent.horizontalCenter
              TlabelText { text: qsTr("input device") }
              ComboBox {
                id: inDevCombo
                width: Noo.fontSize() * 20
                model: SOUND.inputDevices()
                delegate: ItemDelegate { text: modelData }
              }
              TcheckBox {
                visible: false // TODO
                id: jackInChB
                text: "JACK"
                anchors.verticalCenter: parent.verticalCenter
              }
            }
          }

          Tile {
            description: qsTr("Only sounds longer than the selected time will be pitch-detected.<br>Selecting a longer minimum note duration helps avoid capturing fret noise or other unexpected sounds but decreases responsiveness.")
            Row {
              spacing: Noo.fontSize()
              anchors.horizontalCenter: parent.horizontalCenter
              TlabelText { text: qsTr("minimum note duration") }
              SpinBox {
                id: minDurSpin
                from: 50; to: 1000
                stepSize: 10
                editable: true
              }
              TlabelText { text: qsTr("[milliseconds]") }
            }
          }

          Tile {
            description: qsTr("Minimum volume of a sound to be pitch-detected")
            Row {
              spacing: Noo.fontSize()
              anchors.horizontalCenter: parent.horizontalCenter
              Text { text: qsTr("minimum volume"); anchors.verticalCenter: parent.verticalCenter; color: enabled ? activPal.text : disdPal.text }
              Slider {
                anchors.verticalCenter: parent.verticalCenter
                width: Math.min(Noo.fontSize() * 15, parent.parent.width / 3)
                from: 10; to: 80
                value: volSpin.value
                onValueChanged: volSpin.value = value
                stepSize: 5
              }
              SpinBox {
                id: volSpin
                from: 10; to: 80
                value: Math.ceil(GLOB.minVolume * 100)
                editable: true
              }
              TlabelText { text: "%" }
            }
          }

          Tile {
            enabled: false // TODO
            description: qsTr("The base frequency of <i>middle a</i>.<br>Detection of the proper pitch of notes is relative to this value. This also affects the pitch of played sounds.")
            Column {
              width: parent.width
              Row {
                spacing: Noo.fontSize()
                anchors.horizontalCenter: parent.horizontalCenter
                TlabelText { text: qsTr("middle A") }
                Text { // staff
                  height: Noo.fontSize() * 4.5
                  y: Noo.fontSize() * -1.6
                  font { pixelSize: Noo.fontSize() * 2.5; family: "scorek" }
                  text: "\ue014\ue014\ue014\ue014"
                  color: enabled ? activPal.text : disdPal.text
                  Text { // clef
                    x: Noo.fontSize() / 4
                    y: Noo.fontSize() * -0.6
                    color: parent.color
                    font: parent.font
                    text: "\ue050"
                  }
                  Text { // note
                    color: parent.color
                    font: parent.font
                    x: Noo.fontSize() * 2.7
                    y: Noo.fontSize() * -0.9
                    text: "\ue1d7"
                  }
                }
                SpinBox {
                  id: freqSpin
                  anchors.verticalCenter: parent.verticalCenter;
                  from: 391; to: 493 // in range of two semitones up and down around middle A (440Hz)
                  value: 440
                  editable: true
                }
                TlabelText { text: qsTr("[Hz]") }
              }
            }
          }

          Frame { // advanced settings
            width: parent.width * 0.99
            anchors.horizontalCenter: parent.horizontalCenter
            background: Rectangle {
              color: "transparent"
              border.color: enabled ? "red" : disdPal.text
              radius: Noo.fontSize() / 2
            }
            Column {
              width: parent.width
              spacing: Noo.isAndroid() ? 2 : Noo.fontSize() / 2
              Switch {
                id: advSwitch
                text: qsTr("Advanced")
                checked: false
              }
              Tile {
                visible: advSwitch.checked
                Row {
                  anchors.horizontalCenter: parent.horizontalCenter
                  spacing: Noo.fontSize()
                  TlabelText { text: qsTr("pitch detection mode") }
                  ComboBox {
                    id: methodCombo
                    width: Noo.fontSize() * 20
                    model: ["MPM", "autocorrelation", "MPM + modified cepstrum"]
                    delegate: ItemDelegate { text: modelData }
                  }
                }
              }
              Tile {
                visible: advSwitch.checked
                description: qsTr("Helps to properly detect the same notes repeated quickly on the guitar.")
                Row {
                  anchors.horizontalCenter: parent.horizontalCenter
                  spacing: Noo.fontSize()
                  TcheckBox {
                    id: splitVolChB
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("split when volume rise")
                  }
                  SpinBox {
                    id: splitVolSpin
                    from: 5; to: 50
                    enabled: splitVolChB.checked
                    editable: true
                  }
                  TlabelText { text: "%" }
                }
              }
              Tile {
                visible: advSwitch.checked
                description: qsTr("Skips stiller sounds, below given percent of average volume. It prevents detecting of harmonics on classical or acoustic guitar but requires playing with similar strength.")
                Row {
                  anchors.horizontalCenter: parent.horizontalCenter
                  spacing: Noo.fontSize()
                  TcheckBox {
                    id: skipStillerChB
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("skip stiller than")
                  }
                  SpinBox {
                    id: skipStillerSpin
                    from: 10; to: 95
                    enabled: skipStillerChB.checked
                    editable: true
                  }
                  TlabelText { text: "%" }
                }
              }
              Tile {
                visible: advSwitch.checked
                description: qsTr("It is rather necessary for mic input but may be switched off for an instrument plugged line-in with less noise.")
                TcheckBox {
                  id: noiseFilterChB
                  anchors.horizontalCenter: parent.horizontalCenter
                  text: qsTr("noise filter")
                }
              }
            }
          }

        }
      }

    }

    Tflickable { // "playing" page
      contentHeight: outCol.height
      contentWidth: width
      Column {
        id: outCol
        width: parent.width
        spacing: Noo.fontSize()

        TcheckBox {
          id: enableOutChB
          text: qsTr("play sound")
          anchors.horizontalCenter: parent.horizontalCenter
          checked: true
        }

        Column {
          enabled: enableOutChB.checked
          width: parent.width
          spacing: Noo.fontSize()

          Row {
            spacing: Noo.fontSize()
            anchors.horizontalCenter: parent.horizontalCenter
            TlabelText { text: qsTr("output device") }
            ComboBox {
              id: outDevCombo
              width: Noo.fontSize() * 20
              model: SOUND.outputDevices()
              delegate: ItemDelegate { text: modelData }
            }
            TcheckBox {
              visible: false // TODO
              id: jackOutChB
              text: "JACK"
              anchors.verticalCenter: parent.verticalCenter
            }
          }
          Tile {
            description: qsTr("All sounds captured by audio input will be forwarded directly to output device.")
            TcheckBox {
              id: forwardInChB
              anchors.horizontalCenter: parent.horizontalCenter
              text: qsTr("forward input to output")
            }
          }
        }
      }
    }
  }

  Component.onCompleted: {
    if (inDevCombo.currentIndex === -1)
      inDevCombo.model = qsTr("no devices found")
    else
      inDevCombo.currentIndex = inDevCombo.find(SOUND.currentInDevName())
    minDurSpin.value = Math.ceil(GLOB.minDuration * 1000)
    methodCombo.currentIndex = GLOB.detectionMethod
    splitVolChB.checked = GLOB.minSplitVol > 0
    splitVolSpin.value = GLOB.minSplitVol
    skipStillerChB.checked = GLOB.skipStillerVal > 0
    skipStillerSpin.value = GLOB.skipStillerVal
    noiseFilterChB.checked = GLOB.useFilter
    enableInChB.checked = GLOB.audioInEnabled

    if (outDevCombo.currentIndex === -1)
      outDevCombo.model = qsTr("no devices found")
    else
      outDevCombo.currentIndex = outDevCombo.find(SOUND.currentOutDevName())
    forwardInChB.checked = GLOB.forwardInput
    enableOutChB.checked = GLOB.audioOutEnabled
  }

  function save() {
    if (enableInChB.checked) {
      GLOB.inDevName = inDevCombo.currentText
      GLOB.minDuration = minDurSpin.value / 1000.0
      GLOB.minVolume = volSpin.value / 100.0
      GLOB.detectionMethod = methodCombo.currentIndex
      GLOB.minSplitVol = splitVolChB.checked ? splitVolSpin.value : 0
      GLOB.skipStillerVal = skipStillerChB.checked ? skipStillerSpin.value : 0
      GLOB.useFilter = noiseFilterChB.checked
    }
    GLOB.audioInEnabled = enableInChB.checked

    if (enableOutChB.checked) {
      GLOB.outDevName = outDevCombo.currentText
      GLOB.forwardInput = forwardInChB.checked
    }
    GLOB.audioOutEnabled = enableOutChB.checked
    SOUND.acceptSettings()
  }

  function defaults() {
    enableInChB.checked = true
    inDevCombo.currentIndex = 0
    minDurSpin.value = 150
    volSpin.value = 40
    freqSpin.value = 440
    methodCombo.currentIndex = 2
    splitVolChB.checked = true
    splitVolSpin.value = 10
    skipStillerChB.checked = true
    skipStillerSpin.value = 80
    noiseFilterChB.checked = true

    enableOutChB.checked = true
    outDevCombo.currentIndex = 0
    forwardInChB.checked = false
  }
}
