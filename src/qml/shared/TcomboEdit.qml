/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


TcomboBox {
  id: comboEdit

  property alias maximumLength: tf.maximumLength

  implicitWidth: Noo.factor() * 15
  editable: true
  radius: Noo.factor() / 3

  contentItem: TextField {
    id: tf
    text: comboEdit.displayText
    selectByMouse: true
    selectedTextColor: activPal.highlightedText
    selectionColor: activPal.highlight
    background: Rectangle {
      border { width: comboEdit.focus ? 2 : 0; color: activPal.highlight }
      color: enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
      radius: Noo.factor() / 3
    }
  }
}
