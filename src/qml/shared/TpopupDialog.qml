/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Dialog {
  id: dialog

  property alias bgColor: rect.color
  property alias shadowColor: rect.shadowColor
  property alias rejectButton: rejectButton
  property alias acceptButton: acceptButton
  property alias border: rect.border

  width: parent.width * 0.8; height: parent.height * 0.8
  x: (parent.width - width) / 2; y: (parent.height - height) / 2
  background: TipRect {
    id: rect
    color: activPal.window; shadowColor: activPal.shadow
    shadowRadius: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 1)
  }
  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}

  footer: Rectangle {
    color: "transparent"; width: parent.width; height: butRow.height; radius: Noo.fontSize() / 4
    Row {
      id: butRow; spacing: parent.width / 10; padding: parent.height / 7
      anchors.horizontalCenter: parent.horizontalCenter
      TiconButton {
        id: rejectButton
        pixmap: Noo.pix("exit"); text: Noo.TR("QShortcut", "Cancel")
        onClicked: reject()
      }
      TiconButton {
        id: acceptButton
        pixmap: Noo.pix("check"); text: Noo.TR("QPlatformTheme", "Apply")
        onClicked: accept()
      }
    }
  }
}
