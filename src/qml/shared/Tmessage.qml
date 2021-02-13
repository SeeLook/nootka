/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Dialogs 1.2


TpopupDialog {
  property alias message: mText.text
  property color accent: activPal.highlight

  bgColor: Qt.tint(activPal.base, Noo.alpha(accent, 20))
  border { color: accent; width: Noo.factor() / 4.0 }
  glowRect.radius: Noo.factor()
  visible: true; modal: true

  width: mText.width + Noo.factor() / 2
  height: mText.height + header.height + footer.height + Noo.factor()

  rejectButton.visible: false
  acceptButton.text: Noo.TR("QPlatformTheme", "OK")

  Text {
    id: mText
    x: Noo.factor() //; y: (parent.height - height) / 2
    width: Noo.factor() * 40
    wrapMode: Text.WordWrap
    text: modelData
    horizontalAlignment: Text.AlignHCenter
  }

  onClosed: destroy()
}
