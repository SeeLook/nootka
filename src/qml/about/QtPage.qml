/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tflickable {
    contentHeight: text.height

    LinkText {
        id: text

        width: parent.width
        padding: NOO.factor()
        wrapMode: TextEdit.Wrap
        text: dialogObj.aboutQt()
    }

}
