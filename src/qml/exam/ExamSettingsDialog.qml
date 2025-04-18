/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import "../settings"
import Nootka 1.0
import QtQuick 2.12

TpopupDialog {
    id: examSettDialog

    property alias mode: examPage.mode

    visible: true
    width: parent.width * (NOO.isAndroid() ? 0.95 : 0.8)
    height: parent.height * (NOO.isAndroid() ? 0.95 : 0.8)
    caption: qsTranslate("TsettingsDialog", "Simple exam settings")
    onAccepted: examPage.save()

    ExamPage {
        id: examPage

        height: parent.height
    }

}
