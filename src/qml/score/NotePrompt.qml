/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


Text {
  parent: scoreObj.note(0)
  text: "n"
  font { pixelSize: parent.width * 2.2; family: "Nootka" }
  color: Noo.alpha(GLOB.isExam ? GLOB.correctColor : GLOB.noteCursorColor, 50)
  x: (parent.width - width) / 2 - 0.5; y: score.upperLine - 1.6
  visible: !scoreObj.readOnly && parent.notePosY === 0 && !(score.cursor && score.cursor.parent)
}
