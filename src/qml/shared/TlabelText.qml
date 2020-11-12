/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


/** Text usually used in Tile for control label, it is vertically centered and palette aware */
Text {
  anchors.verticalCenter: parent.verticalCenter;
  color: enabled ? activPal.text : disdPal.text
}
