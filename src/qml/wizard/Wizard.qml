/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  SystemPalette {
    id: activPal
    property color dimText: Qt.tint(activPal.base, Noo.alpha(activPal.text, 150))
    colorGroup: SystemPalette.Active
  }
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
        Item {
          AboutPage {
            id: aboutPage
            color: activPal.highlight
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
          HelpPage {
            helpText: HELP.mainHelp()
            enableTOC: false;
            height: parent.height
          }
        }
        Item {
          SoundInfo {}
        }
      }
    }
  }

  footer: Rectangle {
    width: parent.width; height: prevBut.height + Noo.fontSize() + 2
    color: Qt.tint(activPal.window, Noo.alpha(aboutPage.color, 50))
    Rectangle { color: aboutPage.color; height: Noo.fontSize() / 6; width: parent.width; anchors.top: parent.top }
    Row {
      x: (parent.width - width - 8 * Noo.fontSize()) / 2
      anchors.verticalCenter: parent.verticalCenter
      spacing: Noo.fontSize() * 2
      TcuteButton {
        id: prevBut
        anchors.verticalCenter: parent.verticalCenter
        text: Noo.TR("QWizard", "< &Back").replace("&", "")
        enabled: swipe.currentIndex > 0
        onClicked: swipe.currentIndex -= 1
        Shortcut { sequence: StandardKey.MoveToPreviousChar; onActivated: { if (prevBut.enabled) swipe.currentIndex -= 1 }}
      }
      PageIndicator {
        id: pi
        count: swipe.count
        currentIndex: swipe.currentIndex
        anchors.verticalCenter: parent.verticalCenter
        delegate: Rectangle {
          implicitWidth: nextBut.height / 2; implicitHeight: implicitWidth
          radius: width / 2
          color: labelColor
          opacity: index === pi.currentIndex ? 0.95 : (pressed ? 0.7 : 0.45)
          Behavior on opacity { OpacityAnimator { duration: 100 }}
        }
      }
      TcuteButton {
        id: nextBut
        anchors.verticalCenter: parent.verticalCenter
        text: Noo.TR("QWizard", "&Next >").replace("&", "")
        enabled: swipe.currentIndex < swipe.count - 1
        onClicked: swipe.currentIndex += 1
        Shortcut { sequence: StandardKey.MoveToNextChar; onActivated: { if (nextBut.enabled) swipe.currentIndex += 1 }}
      }
    }
    TcuteButton {
      anchors.verticalCenter: parent.verticalCenter
      x: parent.width - width - Noo.fontSize()
      text: Noo.TR("QWizard", "&Finish").replace("&", "")
      onClicked: nootkaWindow.close()
      Shortcut { sequence: "Return"; onActivated: nootkaWindow.close() }
    }
  }

  onClosing: {
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
