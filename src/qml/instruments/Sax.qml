/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Item {
//   anchors.fill: parent
  width: parent.width; height: parent.height * 1.1
  property real ori: height * 0.096 // line.x

  Image { // body
    cache: false
    source: Noo.pix("saxAlto")
    height: parent.height * 0.9
    width: height * 0.715
    x: -width * 0.25
//     y: height * 0.1
    z: -1
  }

//   Rectangle {
//     id: line
//     width: 2; height: parent.height
//     x: parent.height * 0.096
//     color: "blue"
//   }

  SaxFlap { nr: 0; width: parent.height / 34; x: ori - width / 1.2; y: parent.height * 0.1 }
  SaxFlap { nr: 1; x: ori - width / 2; y: parent.height * 0.14 }
  SaxFlap { nr: 2; width: parent.height / 34; x: ori - width / 2; y: parent.height * 0.2 }
  SaxFlap { nr: 3; x: ori - width / 2; y: parent.height * 0.25 }
  SaxFlap { nr: 4; x: ori - width / 2; y: parent.height * 0.32 }

  SaxFlap { x: ori - width / 2; y: parent.height * 0.5 }
  SaxFlap { x: ori - width / 2; y: parent.height * 0.56 }
  SaxFlap { x: ori - width / 2; y: parent.height * 0.62 }
  // trills
  SaxFlap { x: ori + width * 2; y: parent.height * 0.1; width: parent.height / 34; height: width * 2 }
  SaxFlap { x: ori + width * 3; y: parent.height * 0.13; width: parent.height / 34; height: width * 2 }
  SaxFlap { x: ori + width * 2; y: parent.height * 0.17; width: parent.height / 34; height: width * 2 }

  SaxFlap { x: ori + width; y: parent.height * 0.4; width: parent.height / 18; height: width / 2 }
  SaxFlap { x: ori + width * 1.2; y: parent.height * 0.43; width: parent.height / 26; height: width / 2 }
  SaxFlap { x: ori + width * 2.2; y: parent.height * 0.43; width: parent.height / 26; height: width / 2 }
  SaxFlap { x: ori + width; y: parent.height * 0.45; width: parent.height / 18; height: width / 2 }

  SaxFlap { x: ori - width * 2.5; y: parent.height * 0.4; width: parent.height / 34; height: width * 2; radius: 0 }
  SaxFlap { x: ori - width * 2.5; y: parent.height * 0.46; width: parent.height / 34; height: width * 1.8; radius: 0 }
  SaxFlap { x: ori - width * 2.5; y: parent.height * 0.515; width: parent.height / 34; height: width * 1.5; radius: 0 }

  SaxFlap { x: ori - width * 2.5; y: parent.height * 0.6; width: parent.height / 34; height: width * 2; radius: 0; rotation: 10 }
  SaxFlap { x: ori - width * 2.6; y: parent.height * 0.66; width: parent.height / 34; height: width * 2; radius: 0; rotation: 10 }

  SaxFlap { x: ori - width * 1.5; y: parent.height * 0.75; width: parent.height / 18; height: width / 2; rotation: 30 }
  SaxFlap { x: ori - width * 1.5; y: parent.height * 0.78; width: parent.height / 18; height: width / 2; rotation: 30 }

  OutScaleTip { visible: false }
}
