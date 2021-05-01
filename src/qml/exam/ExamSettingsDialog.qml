/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import "../settings"


TpopupDialog {
  id: examSettDialog
  property alias mode: examPage.mode
  visible: true

  width: parent.width * (NOO.isAndroid() ? 0.95 : 0.8)
  height: parent.height * (NOO.isAndroid() ? 0.95 : 0.8)

  caption: qsTranslate("TsettingsDialog", "Simple exam settings")

  ExamPage {
    id: examPage
    height: parent.height
  }

  onAccepted: examPage.save()
}
