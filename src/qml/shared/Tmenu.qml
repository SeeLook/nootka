/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12


Menu {
  width: NOO.factor() * 20
  scale: GLOB.useAnimations ? 0.1 : 1.0

  background: GlowRect { shadowRadius: NOO.factor() / 2; color: activPal.window }

  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}
}
