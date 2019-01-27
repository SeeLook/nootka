/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


NoteCursor {
  width: parent ? parent.width - (scoreObj.singleNote ? 0 : parent.alterWidth) : 0

  // private
  property bool allow: true

  visible: yPos > 0 && allow && (scoreObj.singleNote || scoreObj.editMode)

  Connections { // hide cursor after click on a note to show what was selected
    target: scoreObj
    onClicked: allow = false
    onActiveYposChanged: allow = scoreObj.activeYpos > 0
  }

}
