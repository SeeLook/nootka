/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2025 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2

import Nootka 1.0
import "../"


Tflickable {
  id: globalPage
  height: parent.height

  contentHeight: mainColumn.height + NOO.factor() * 2

  Column {
    id: mainColumn
    spacing: NOO.isAndroid() ? 2 : NOO.factor() / 2
    width: parent.width

    Tile {
      description: qsTr("Select a language.<br><span style=\"color: red;\">To take effect, this requires restarting the application!</span>")

      ListModel {
        id: langModel
        ListElement { flag: "default";  lang: QT_TR_NOOP("default") }
        ListElement { flag:"cs"; lang: "Český" }
        ListElement { flag:"de"; lang: "Deutsch" }
        ListElement { flag:"en"; lang: "English" }
        ListElement { flag:"es"; lang: "Español" }
        ListElement { flag:"fr"; lang: "Français" }
        ListElement { flag:"hu"; lang: "Magyar" }
        ListElement { flag:"it"; lang: "Italiano" }
        ListElement { flag:"pl"; lang: "polski" }
        ListElement { flag:"br"; lang: "Português" }
        ListElement { flag:"ru"; lang: "Русский" }
        ListElement { flag:"sl"; lang: "Slovenščina" }
        ListElement { flag:"ta"; lang: "தமிழ்" }
        ListElement { flag:"uk"; lang: "Українська" }
      }

      Tumbler {
        id: langTumb
        width: parent.width
        height: NOO.factor() * (NOO.isAndroid() ? 6 : 8)
        visibleItemCount: Math.min(((width / (NOO.factor() * (NOO.isAndroid() ? 5.5 : 7))) / 2) * 2 - 1, 7)
        model: langModel
        delegate: Component {
          Column {
            spacing: NOO.factor() / 4
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            Image {
              source: NOO.pix("flags/" + flag)
              height: langTumb.height * 0.375; width: height * (sourceSize.height / sourceSize.width)
              anchors.horizontalCenter: parent.horizontalCenter
              MouseArea {
                anchors.fill: parent
                onClicked: langTumb.currentIndex = index
              }
            }
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: flag === "default" ? qsTr(lang) : lang
              color: activPal.text
              font { bold: langTumb.currentIndex === index; pixelSize: langTumb.height * 0.1 }
            }
          }
        }
        contentItem: PathView {
          id: pathView
          model: langTumb.model
          delegate: langTumb.delegate
          clip: true
          pathItemCount: langTumb.visibleItemCount + 1
          preferredHighlightBegin: 0.5
          preferredHighlightEnd: 0.5
          dragMargin: width / 2
          path: Path {
            startX: 0
            startY: NOO.factor() * 1.4
            PathLine {
              x: pathView.width
              y: NOO.factor() * 1.4
            }
          }
        }
        Rectangle {
          z: -1; width: parent.height * 1.1; height: parent.height * 0.7
          x: parent.width / 2 - width / 2; y: parent.height * 0.01
          color: NOO.alpha(activPal.highlight, 100)
          radius: width / 12
        }
      }
    }

    Tile {
      TcheckBox {
        id: animChBox
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("enable animations")
        checked: GLOB.useAnimations
      }
    }

    Tile {
      TcheckBox {
        id: hintsChBox
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("show hints")
        checked: GLOB.showHints
      }
    }

    Tile {
      Column {
        spacing: NOO.factor()
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter
        TcuteButton {
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Check for updates")
          onClicked: dialogObj.checkForUpdates()
        }
        Text {
          color: activPal.text
          height: NOO.factor() * 3; width: parent.width; verticalAlignment: Text.AlignVCenter
          text: dialogObj.updateMessage; wrapMode: Text.WordWrap
        }
      }
    }

    Tile {
      description: qsTr("Scaling factor of visible texts and others GUI elements.<br>Requires application restart.")
      Row {
        spacing: NOO.factor() * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          color: activPal.text; width: NOO.factor() * text.length
          text: qsTr("scale")
          anchors.verticalCenter: parent.verticalCenter
          font.pixelSize: (NOO.factor() / GLOB.scale) * scaleSlider.value
        }
        Tslider {
          id: scaleSlider
          from: 0.5; to: 1.5; stepSize: 0.1; snapMode: Slider.SnapAlways
          value: GLOB.scale
          width: Math.min(NOO.factor() * 15, globalPage.width * 0.4)
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.round(scaleSlider.value * 100) + "%"
          font.pixelSize: (NOO.factor() / GLOB.scale) * scaleSlider.value
          color: activPal.text; width: NOO.factor() * 3; horizontalAlignment: Text.AlignHCenter
        }
      }
    }

    Item { height: NOO.factor() * 3; width: parent.width }

    Tile {
      description: qsTr("All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard.")
      descriptionColor: "red"
      TcuteButton {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Restore all default settings")
        onClicked: restoreDialog.open()
      }
    }

  }

  TpopupDialog {
    id: restoreDialog
    modal: true
    width: Math.min(NOO.factor() * 50, globalPage.width * 0.9)
    height: rmmConfTxt.height + NOO.factor() * 8
    bgColor: Qt.tint(activPal.window, NOO.alpha("red", 20))
    border { color: "red"; width: NOO.factor() / 4.0 }
    caption: qsTr("Restore all default settings")
    Text {
      id: rmmConfTxt
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width - NOO.factor() * 2
      color: activPal.text
      font.pixelSize: NOO.factor() * 1.2
      text: qsTr("All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard.")
      wrapMode: Text.WordWrap
    }
    onAccepted: {
      NOO.setResetConfig(true)
      Qt.quit()
    }
  }

  Timer { // workaround to select 0 item, call it with delay
    id: langTimer
    running: true
    interval: 50
    onTriggered: {
      for (var i = 0; i < langModel.count; ++i) {
        if (langModel.get(i).flag === GLOB.lang || (i == 0 && GLOB.lang === "")) {
          langTumb.currentIndex = i + 1 // FIXME: workaround for Qt 5.10.1
          langTumb.currentIndex = i
          break
        }
      }
    }
  }

  function save() {
    GLOB.useAnimations = animChBox.checked
    GLOB.showHints = hintsChBox.checked
    GLOB.lang = langTumb.currentIndex === 0 ? "" : langModel.get(langTumb.currentIndex).flag
    GLOB.scale = scaleSlider.value
  }

  function defaults() {
    animChBox.checked = true
    hintsChBox.checked = true
    langTumb.currentIndex = 0
    scaleSlider.value = 1.0
  }

  function help() { NOO.openDocLink("2017/05/17/settings/") }

  property var updater: null
  Connections {
    target: dialogObj
    onUpdateSummary: {
      if (!updater)
        updater = Qt.createComponent("qrc:/updater/UpdaterPopup.qml").createObject(globalPage)
      updater.open()
    }
  }
}
