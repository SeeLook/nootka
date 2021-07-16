/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12


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

  property var intonationTexts: [
      qsTranslate("TintonationCombo", "intonation accuracy"),
      qsTranslate("TintonationCombo", "do not check"),
      qsTranslate("TintonationCombo", "gums pain", "We used to say that gums or teeth are paining when music is out of tune but it isn't official term."),
      qsTranslate("TintonationCombo", "violinist beginner"),
      qsTranslate("TintonationCombo", "old strings"),
      qsTranslate("TintonationCombo", "well tuned instrument"),
      qsTranslate("TintonationCombo", "perfect"),
      qsTranslate("TintonationCombo", "cents", "unit of measure used for musical intervals. This text always is used in context: ' ± [5, 10, 20, 30] cents '")
  ]
}
