/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


TpopupDialog {
  id: melNamDial

  visible: true
  width: Noo.fontSize() * 40; height: Noo.fontSize() * 12
  x: (parent.width - width) / 2
  y: Noo.isAndroid() ? Noo.fontSize() : (parent.height - height) / 2

  rejectButton.text: Noo.TR("QPlatformTheme", "Discard")
  acceptButton.text: Noo.TR("QShortcut", "Save")
  acceptButton.pixmap: Noo.pix("save")

  Column {
    anchors.centerIn: parent
    spacing: Noo.fontSize()
    Row {
      anchors { right: parent.right; rightMargin: Noo.fontSize() / 2 }
      spacing: Noo.fontSize()
      Text {
        anchors.verticalCenter: parent.verticalCenter
        color: activPal.text
        text: qsTr("Title")
      }
      TcomboEdit {
        id: melodyTitle
        maximumLength: 100
        model: mainObj.recentTitles() /**< @p mainObj is TmainScoreObject instance */
        width: melNamDial.width - Noo.fontSize() * 11
      }
    }

    Row {
      anchors { right: parent.right; rightMargin: Noo.fontSize() / 2 }
      spacing: Noo.fontSize()
      Text {
        anchors.verticalCenter: parent.verticalCenter
        color: activPal.text
        text: qsTr("Composer")
      }
      TcomboEdit {
        id: composer
        maximumLength: 100
        model: mainObj.recentComposers()
        width: melNamDial.width - Noo.fontSize() * 11
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
    mainObj.saveMusicXml(/*fileName.text*/"", melodyTitle.editText, composer.editText)
    destroy(300)
  }
  onRejected: destroy(300)

  onOpened: SOUND.stopListen()
  onClosed: SOUND.startListen()
}
