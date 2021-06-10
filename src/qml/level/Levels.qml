/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


/**
 * This QML file is fake...
 * It exists only to be parsed by lupdate!
 * Its code is never used (included in build process)
 *
 * It contains translatable texts for names and descriptions of level files shipped with Nootka.
 * So 'Levels' is translation context where lupdate will put the texts in.
 */
Item {
  property string name1: qsTr("J. S. Bach - Minuet")
  property string desc1: qsTr("Play melody of J. S. Bach Minuet from Notebook for Anna Magdalena Bach divided into smaller parts.")

  property string name2: qsTr("Simple melodies")
  property string desc2: qsTr("Play popular, simple melodies on your instrument.")
}
