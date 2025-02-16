/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Dialogs 1.2

TpopupDialog {
    property alias message: mText.text
    property color accent: activPal.highlight

    bgColor: Qt.tint(activPal.base, NOO.alpha(accent, 20))
    glowRect.radius: NOO.factor()
    visible: true
    modal: true
    width: mText.width + NOO.factor() / 2
    height: mText.height + header.height + footer.height + NOO.factor()
    rejectButton.visible: false
    acceptButton.text: NOO.TR("QPlatformTheme", "OK")
    onClosed: destroy()

    border {
        color: accent
        width: NOO.factor() / 4
    }

    Text {
        id: mText

        x: NOO.factor() //; y: (parent.height - height) / 2
        width: NOO.factor() * 40
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

}
