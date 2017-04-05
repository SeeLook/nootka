/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */


/**
 * It masks standard translation function to skip string during lupdate.
 * This way texts from qt translations can be reused without being doubled in Nootka translation
 */
function tr(context, source) {
  return qsTranslate(context, source)
}
