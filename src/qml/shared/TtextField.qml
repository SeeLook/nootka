/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12


TextField {
  id: tf

  property alias bg: bg

  selectByMouse: true
  selectedTextColor: activPal.highlightedText
  selectionColor: activPal.highlight
  placeholderTextColor: disdPal.text
  color: enabled ? activPal.text : disdPal.text

  background: TipRect {
    id: bg
    border { width: tf.focus ? 1 : 0; color: activPal.highlight }
    color: enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
  }
}
