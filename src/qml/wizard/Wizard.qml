/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../about"
import "../"


ApplicationWindow {
  id: nootkaWindow
  title: "Nootka - " + qsTr("First run wizard")
  visible: true
  color: activPal.window
  width: GLOB.geometry.width
  height: GLOB.geometry.height
  x: GLOB.geometry.x
  y: GLOB.geometry.y

  property alias instrument: instrPage.instrument
  property alias swipe: swipe
  property alias labelColor: aboutPage.color

  readonly property int fontSize: Noo.fontSize()

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  Row {
    anchors.fill: parent
    Item {
      height: nootkaWindow.height - footer.height; width: height * 0.246
      Image {
        id: leftImg
        source: Noo.pix("wizard-left")
        height: nootkaWindow.height - footer.height; width: height * 0.246
      }
    }
    Item {
      height: parent.height; width: parent.width - parent.height * 0.246
      SwipeView {
        id: swipe
        anchors.fill: parent
        clip: true
//         Item {
//           SoundInfo {}
//         }

        Item {
          AboutPage {
            id: aboutPage
            color: "#00a0a0"
          }
        }
        Item {
          WizardInstrument { id: instrPage }
        }
        Item {
          WizardClef { id: clefPage }
        }
        Item {
          WizardOptions { id: optionsPage }
        }
        Item {
          HelpPage { helpText: HELP.mainHelp(); height: parent.height }
        }
        Item {
          SoundInfo {}
        }
      }
    }
  }

  PageIndicator {
    count: swipe.count
    currentIndex: swipe.currentIndex
    anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom }
  }

  footer: Rectangle {
    visible: !Noo.isAndroid()
    width: parent.width; height: prevBut.height + fontSize + 2
    color: Qt.tint(activPal.window, Noo.alpha(aboutPage.color, 50)) // Qt.darker(activPal.window, 0.9)
    Rectangle { color: aboutPage.color; height: fontSize / 6; width: parent.width; anchors.top: parent.top }
    TcuteButton {
      id: prevBut
      anchors.verticalCenter: parent.verticalCenter
      x: parent.width / 2 - width - fontSize
      text: Noo.TR("QWizard", "< &Back").replace("&", "")
      enabled: swipe.currentIndex > 0
      onClicked: swipe.currentIndex -= 1
      Shortcut { sequence: StandardKey.MoveToPreviousChar; onActivated: { if (prevBut.enabled) swipe.currentIndex -= 1 }}
    }
    TcuteButton {
      id: nextBut
      anchors.verticalCenter: parent.verticalCenter
      x: parent.width / 2 + fontSize
      text: Noo.TR("QWizard", "&Next >").replace("&", "")
      enabled: swipe.currentIndex < swipe.count - 1
      onClicked: swipe.currentIndex += 1
      Shortcut { sequence: StandardKey.MoveToNextChar; onActivated: { if (nextBut.enabled) swipe.currentIndex += 1 }}
    }
    TcuteButton {
      anchors.verticalCenter: parent.verticalCenter
      x: parent.width - width - fontSize
      text: Noo.TR("QWizard", "&Finish").replace("&", "")
      onClicked: nootkaWindow.close()
      Shortcut { sequence: "Return"; onActivated: nootkaWindow.close() }
    }
  }

  onClosing: {
    console.log("Wizard is closing:", Noo.instr(instrPage.getInstrument()).name)
    GLOB.setInstrument(instrPage.getInstrument())
    clefPage.setInstrParams()
    optionsPage.setOptions()
    GLOB.keyNameStyle = (Noo.keyNameTranslated() !== "letters" ? (Qt.locale().name.indexOf("ru") === -1 ? 2 : 5) : (GLOB.seventhIsB ? 3 : 0))
    GLOB.updateKeySignatureNames()
    GLOB.audioInstrument = instrPage.getInstrument()
    GLOB.preferFlats = GLOB.instrument.isSax ? true : false
    Qt.quit()
  }
}
