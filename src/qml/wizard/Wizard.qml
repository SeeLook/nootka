/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import "../about"
import "../fakeTrans.js" as Fake


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

  readonly property int fontSize: nootkaWindow.font.pixelSize

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  SwipeView {
    id: swipe
    anchors.fill: parent

    Item {
        AboutPage {}
//         Image {
//           source: TcheckBoxpix("wizard-left")
//           height: nootkaWindow.height
//           width: nootkaWindow.height * 0.246
//         }
    }
    WizardInstrument {
      id: instrPage
    }
    WizardClef { id: clefPage }
    WizardOptions { id: optionsPage }
    Item {
      Text { text: "Some help"; anchors.centerIn: parent }
    }
  }

  footer: Column {
    width: parent.width;
    PageIndicator {
      count: swipe.count
      currentIndex: swipe.currentIndex
      anchors.horizontalCenter: parent.horizontalCenter
    }
    Item {
      visible: !Noo.isAndroid()
      width: parent.width; height: prevBut.height
      Button {
        x: parent.width / 2 - width - fontSize
        id: prevBut
        text: Fake.tr("QWizard", "< &Back").replace("&", "")
        enabled: swipe.currentIndex > 0
        onClicked: swipe.currentIndex -= 1
      }
      Button {
        x: parent.width / 2 + fontSize
        text: Fake.tr("QWizard", "&Next >").replace("&", "")
        enabled: swipe.currentIndex < swipe.count - 1
        onClicked: swipe.currentIndex += 1
      }
      Button {
        anchors.right: parent.right
        text: Fake.tr("QWizard", "&Finish").replace("&", "")
        onClicked: nootkaWindow.close()
      }
    }
  }

  onClosing: {
    console.log("Wizard is closing:", Noo.instr(instrPage.instrument).name)
    GLOB.setInstrument(instrPage.instrument)
    GLOB.clefType = clefPage.clef
    GLOB.transposition = clefPage.transposition
    GLOB.seventhIsB = optionsPage.is7B
    GLOB.enableDoubleAccids = optionsPage.doubleAccids
    GLOB.keySignatureEnabled - optionsPage.useKeys
    GLOB.showEnharmNotes = optionsPage.enharmNames
    Qt.quit()
  }
//   Component.onDestruction: console.log("Wizard Bye")
}
