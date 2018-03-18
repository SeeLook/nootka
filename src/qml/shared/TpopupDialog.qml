/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

// import Nootka.Dialogs 1.0


Dialog {
  id: dialog
  property alias bgColor: rect.color
  property alias shadowColor: rect.shadowColor

  width: parent.width * 0.8; height: parent.height * 0.8
  x: (parent.width - width) / 2; y: (parent.height - height) / 2
  background: TipRect { id: rect; color: activPal.window; shadowRadius: Noo.fontSize(); shadowColor: activPal.shadow }
  scale: GLOB.useAnimations ? 0.1 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0.1 }}

//   TdialogObject { id: dialogObj }

//   footer: DialogButtonBox {
//     id: box
//     visible: standardButtons != 0
//     width: parent.width; height: Noo.fontSize() * 3
//     padding: Noo.fontSize() / 4
//     spacing: Noo.fontSize()
//     delegate: TiconButton {
//       pixmap: Noo.pix(dialogObj.buttonRoleIcon(DialogButtonBox.buttonRole))
//     }
//     background: Rectangle {
//       anchors.fill: parent
//       color: Qt.darker(dialog.bgColor, 1.1)
//     }
//     onClicked: {
//       switch (button.DialogButtonBox.buttonRole) {
//         case DialogButtonBox.AcceptRole: dialog.accept(); break
// //         case DialogButtonBox.HelpRole: dialog.help(); break
// //         case DialogButtonBox.ResetRole: dialog.reset(); break
//         case DialogButtonBox.ApplyRole: dialog.accept(); break
//         case DialogButtonBox.ActionRole: break
//         default: dialog.close()
//       }
//     }
//   }

  footer: Rectangle {
    color: dialog.background.color; width: parent.width; height: butRow.height; radius: Noo.fontSize() / 4
    Row {
      id: butRow; spacing: parent.width / 10; padding: parent.height / 7
      anchors.horizontalCenter: parent.horizontalCenter
      TiconButton {
        pixmap: Noo.pix("exit"); text: Noo.TR("QShortcut", "Cancel")
        onClicked: reject()
      }
      TiconButton {
        pixmap: Noo.pix("check"); text: Noo.TR("QPlatformTheme", "Apply")
        onClicked: accept()
      }
    }
  }
}
