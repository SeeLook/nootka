/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import Nootka.Music

NoteCursor {
    // private
    property bool allow: true

    width: parent ? parent.width - (scoreObj.singleNote ? 0 : parent.alterWidth) : 0
    visible: yPos > 0 && allow && (scoreObj.singleNote || scoreObj.editMode)

    // hide cursor after click on a note to show what was selected
    Connections {
        target: scoreObj
        function onClicked() : void { allow = false }
        function onActiveYposChanged() : void { allow = scoreObj.activeYpos > 0 }
    }

}
