/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


/**
 * Visibility of this control depends on active (cursor) note of the score
 */
ControlBase {
  id: accidControl

  property int selectedId: -1
  property string text: ""
  property int alter: accidArray[selectedId + 1]
  readonly property var accidGlyphs: [ "\ue264", "\ue260", "\ue262", "\ue263" ]
  readonly property var accidArray: [ 0, -2, -1, 1, 2 ]


  x: (show ? score.scoreObj.xFirstInActivBar : -Noo.fontSize()) - width
  y: score.contentY + (score.height - height) / 2

  component: Component {
      Column {
          Repeater {
            model: 4
            ControlButton {
              factor: accidControl.factor
              yOffset: factor * -4.4
              visible: score.enableDoubleAccids || index === 1 || index === 2
              selected: accidControl.selectedId === index
              font { family: "scorek"; pixelSize: factor * 3 }
              text: accidGlyphs[index]
              onClicked: {
                selectedId = selectedId === index ? -1 : index
                accidControl.text = selectedId > -1 ? accidGlyphs[selectedId] : ""
              }
              onEntered: hideTimer.stop()
              onExited: hideTimer.restart()
            }
          }
      }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}

}
