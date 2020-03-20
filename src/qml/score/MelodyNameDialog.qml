/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


TpopupDialog {
  id: melNamDial

  visible: true
  width: Noo.fontSize() * 40; height: Noo.fontSize() * 16

  rejectButton.text: Noo.TR("QPlatformTheme", "Discard")
  acceptButton.text: Noo.TR("QShortcut", "Save")
  acceptButton.pixmap: Noo.pix("save")

  Column {
    anchors.centerIn: parent
    spacing: Noo.fontSize()
    Row {
      anchors.right: parent.right
      spacing: Noo.fontSize()
      Text {
        anchors.verticalCenter: parent.verticalCenter
        color: activPal.text
        text: qsTr("Title")
      }
      TtextField {
        id: melodyTitle
        font.pixelSize: Noo.fontSize(); maximumLength: 100
        width: melNamDial.width - Noo.fontSize() * 10
        text: qsTr("Nootka melody")
      }
    }

    Row {
      anchors.right: parent.right
      spacing: Noo.fontSize()
      Text {
        anchors.verticalCenter: parent.verticalCenter
        color: activPal.text
        text: qsTr("Composer")
      }
      TtextField {
        id: composer
        font.pixelSize: Noo.fontSize(); maximumLength: 100
        width: melNamDial.width - Noo.fontSize() * 10
        text: "Nootka The Composer"
      }
    }
    /**
     * FIXME
     * Code below displays fileName line with button to open file save dialog,
     * but when user selects some file name from that dialog it locks further title & composer changes for file name.
     * Also when "save" button is invoked it may overwrite any existing file with the same name without any asking
     * So
     * keep two dialogues way so far:
     * - name/composer QML dialog
     * - then save file dialog
     */
//     Row {
//       anchors.right: parent.right
//       spacing: Noo.fontSize()
//       Text {
//         anchors.verticalCenter: parent.verticalCenter
//         color: activPal.text
//         text: Noo.TR("QFileDialog", "File")
//       }
//       Text {
//         id: fileName
//         anchors.verticalCenter: parent.verticalCenter
//         font { pixelSize: Noo.fontSize(); bold: true }
//         width: melNamDial.width - Noo.fontSize() * 14
//         horizontalAlignment: Text.AlignHCenter; elide: Text.ElideMiddle
//         text: GLOB.lastXmlDir() + (Noo.isWindows() ? "\\" : "/") + composer.text + " - " + melodyTitle.text + ".musicxml"
//       }
//       TiconButton {
//         pixmap: Noo.pix("open")
//         onClicked: {
//           var f = Noo.getXmlToSave(composer.text + " - " + melodyTitle.text)
//           if (f !== "")
//             fileName.text = f
//         }
//       }
//     }
  }

  onAccepted: {
    mainObj.saveMusicXml(/*fileName.text*/"", melodyTitle.text, composer.text)
    destroy(300)
  }
  onRejected: {
    destroy(300)
  }

  onOpened: SOUND.stopListen()
  onClosed: SOUND.startListen()
}
