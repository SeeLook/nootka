/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Dialogs

TcuteButton {
    id: colorButton

    property alias title: colorDialog.title

    implicitWidth: NOO.factor() * 3.5
    implicitHeight: NOO.factor() * 2
    onClicked: colorDialog.open()
    contentItem: null
    checked: !enabled

    // disable cover
    Rectangle {
        anchors.fill: background
        visible: !colorButton.enabled
        color: disdPal.text
        radius: NOO.factor() / 3
    }

    ColorDialog {
        id: colorDialog

        selectedColor: colorButton.color
        onAccepted: colorButton.color = colorDialog.selectedColor
        modality: Qt.WindowModal
        options: ColorDialog.ShowAlphaChannel | ColorDialog.NoEyeDropperButton
    }

    Behavior on color {
        enabled: GLOB.useAnimations

        ColorAnimation {
            duration: 750
        }

    }

}
