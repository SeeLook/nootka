/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import "../"


Tflickable {
  id: globalPage
  height: parent.height

  contentHeight: mainColumn.height + Noo.factor() * 2

  Column {
    id: mainColumn
    spacing: Noo.isAndroid() ? 2 : Noo.factor() / 2
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
        ListElement { flag:"ru"; lang: "Русский" }
        ListElement { flag:"sl"; lang: "Slovenščina" }
      }

      Tumbler {
        id: langTumb
        width: parent.width
        height: Noo.factor() * 8
        visibleItemCount: Math.min(((width / (Noo.factor() * 7)) / 2) * 2 - 1, 7)
        model: langModel
        delegate: Component {
          Column {
            spacing: Noo.factor() / 4
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            scale: 1.7 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            Image {
              source: Noo.pix("flags/" + flag)
              height: Noo.factor() * 3; width: height * (sourceSize.height / sourceSize.width)
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
              font { bold: langTumb.currentIndex === index; pixelSize: Noo.factor() * 0.8 }
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
            startY: Noo.factor() * 1.4
            PathLine {
              x: pathView.width
              y: Noo.factor() * 1.4
            }
          }
        }
        Rectangle {
          z: -1; width: Noo.factor() * 9; height: parent.height * 0.7
          x: parent.width / 2 - width / 2; y: 2 //-parent.height * 0.05
          color: Noo.alpha(activPal.highlight, 100)
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
        spacing: Noo.factor()
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter
        TcuteButton {
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Check for updates")
          onClicked: dialogObj.checkForUpdates()
        }
        Text {
          color: activPal.text
          height: Noo.factor() * 3; width: parent.width; verticalAlignment: Text.AlignVCenter
          text: dialogObj.updateMessage; wrapMode: Text.WordWrap
        }
      }
    }

    Tile {
      description: qsTr("Scaling factor of visible texts and others GUI elements.<br>Requires application restart.")
      Row {
        spacing: Noo.factor() * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          color: activPal.text; width: Noo.factor() * text.length
          text: qsTr("scale")
          anchors.verticalCenter: parent.verticalCenter
          font.pixelSize: (Noo.factor() / GLOB.scale) * scaleSlider.value
        }
        Tslider {
          id: scaleSlider
          from: 0.5; to: 1.5; stepSize: 0.1; snapMode: Slider.SnapAlways
          value: GLOB.scale
          width: Math.min(Noo.factor() * 15, globalPage.width * 0.4)
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.round(scaleSlider.value * 100) + "%"
          font.pixelSize: (Noo.factor() / GLOB.scale) * scaleSlider.value
          color: activPal.text; width: Noo.factor() * 3; horizontalAlignment: Text.AlignHCenter
        }
      }
    }

    Item { height: Noo.factor() * 3; width: parent.width }

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
    width: Math.min(Noo.factor() * 50, globalPage.width * 0.9)
    height: rmmConfTxt.height + Noo.factor() * 8
    bgColor: Qt.tint(activPal.window, Noo.alpha("red", 20))
    border { color: "red"; width: Noo.factor() / 4.0 }
    caption: qsTr("Restore all default settings")
    Text {
      id: rmmConfTxt
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width - Noo.factor() * 2
      color: activPal.text
      font.pixelSize: Noo.factor() * 1.2
      text: qsTr("All settings will be reset to their default values!<br>Nootka will start up with the first-run wizard.")
      wrapMode: Text.WordWrap
    }
    onAccepted: {
      Noo.setResetConfig(true)
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

  function help() { Noo.openHelpLink("settings") }

}
