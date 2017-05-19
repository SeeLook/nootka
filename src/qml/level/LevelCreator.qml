/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7


Item {
  width: parent.width
  height: parent.height - (Noo.isAndroid() ? 0 : nootkaWindow.fontSize * 3)

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("levelsSett", qsTranslate("TlevelSelector", "Levels"), "level/Levels")
    pages.addItem("questionsSett", qsTranslate("TlevelCreatorDlg", "Questions"), "level/Questions")
    pages.addItem("melodySett", qsTr("Melody"), "level/Melody")
    pages.addItem("accidSett", qsTranslate("TlevelSelector", "Accidentals"),  "level/Accids")
    pages.addItem("rangeSett", qsTranslate("TlevelSelector", "Range"),  "level/Range")
  }
}
