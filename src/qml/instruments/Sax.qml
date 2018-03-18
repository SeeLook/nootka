/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


TsaxBg {

  // private
  property real xAxis: height * 0.096 // line.x

  width: parent.width; height: parent.height * 1.1

  Image { // body
    cache: false
    source: Noo.pix("saxAlto")
    height: parent.height * 0.9
    width: height * (sourceSize.width/ sourceSize.height)
    x: -width * 0.25
    z: -1
  }


//   Rectangle {
//     id: line
//     width: 2; height: parent.height
//     x: parent.height * 0.096
//     color: "blue"
//   }

  // left fingers
  SaxFlap { nr: 0; width: parent.height / 34; x: xAxis - width; y: parent.height * 0.1 }
  SaxFlap { nr: 1; x: xAxis - width / 2; y: parent.height * 0.14 }
  SaxFlap { nr: 2; width: parent.height / 34; x: xAxis; y: parent.height * 0.2 }
  SaxFlap { nr: 3; x: xAxis - width / 2; y: parent.height * 0.25 }
  SaxFlap { nr: 4; x: xAxis - width / 2; y: parent.height * 0.32 }
  // left side flaps
  SaxFlap { nr: 5; x: xAxis - width / 2; y: parent.height * 0.5 }
  SaxFlap { nr: 6; x: xAxis - width / 2; y: parent.height * 0.56 }
  SaxFlap { nr: 7; x: xAxis - width / 2; y: parent.height * 0.62 }
  // left pinkie cluster
  SaxFlap { nr: 8; x: xAxis + width * 1.5; y: parent.height * 0.17; width: parent.height / 34; height: width * 2; rotation: -10 }
  SaxFlap { nr: 9; x: xAxis + width * 2.25; y: parent.height * 0.13; width: parent.height / 34; height: width * 2; rotation: -10 }
  SaxFlap { nr: 10; x: xAxis + width * 3.25; y: parent.height * 0.17; width: parent.height / 34; height: width * 2; rotation: -10 }
  // left thumb
  SaxFlap { nr: 11; x: xAxis - width * 2; width: parent.height / 22; height: width * 0.8 ; y: parent.height * 0.22 }

  // right hand
  SaxFlap { nr: 12; x: xAxis + width; y: parent.height * 0.4; width: parent.height / 18; height: width / 2 }
  SaxFlap { nr: 13; x: xAxis + width * 1.2; y: parent.height * 0.43; width: parent.height / 26; height: width / 2 }
  SaxFlap { nr: 14; x: xAxis + width * 2.2; y: parent.height * 0.43; width: parent.height / 26; height: width / 2 }
  SaxFlap { nr: 15; x: xAxis + width; y: parent.height * 0.45; width: parent.height / 18; height: width / 2 }
  // right hand side flaps
  SaxFlap { nr: 16; x: xAxis - width * 2.5; y: parent.height * 0.4; width: parent.height / 34; height: width * 2; radius: 0 }
  SaxFlap { nr: 17; x: xAxis - width * 2.5; y: parent.height * 0.46; width: parent.height / 34; height: width * 1.8; radius: 0 }
  SaxFlap { nr: 18; x: xAxis - width * 2.5; y: parent.height * 0.515; width: parent.height / 34; height: width * 1.5; radius: 0 }
  // ...
  SaxFlap { nr: 19; x: xAxis - width * 2.5; y: parent.height * 0.6; width: parent.height / 34; height: width * 2; radius: 0; rotation: 10 }
  SaxFlap { nr: 20; x: xAxis - width * 2.6; y: parent.height * 0.66; width: parent.height / 34; height: width * 2; radius: 0; rotation: 10 }
  // right pinkie flaps
  SaxFlap { nr: 21; x: xAxis - width * 1.5; y: parent.height * 0.75; width: parent.height / 18; height: width / 2; rotation: 30 }
  SaxFlap { nr: 22; x: xAxis - width * 1.5; y: parent.height * 0.78; width: parent.height / 18; height: width / 2; rotation: 30 }

  OutScaleTip { visible: !active && outOfScale; width: parent.width * 0.9 }

  TextEdit {
    anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter
    font.pixelSize: 30
    text: fingeringId
  }
}
