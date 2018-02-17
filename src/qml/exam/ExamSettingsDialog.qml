/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"
import "../settings"


TpopupDialog {
  id: examSettDialog
  property alias mode: examPage.mode
  visible: true

  ExamPage {
    id: examPage
    height: parent.height
  }

  onAccepted: examPage.save()
}
