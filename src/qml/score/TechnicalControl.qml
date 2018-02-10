/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


ControlBase {
  id: techControl
  y: accidControl.y
  visible: show

  component: Component {
    Row {
      Repeater {
          model: [ "\uE610", "\uE612" ]
          ControlButton {
            factor: accidControl.factor * 0.9
            height: factor * 2
            yOffset: -factor * 2.5
            font { family: "Scorek"; pixelSize: factor * 2 }
            text: modelData
            onClicked: {
              techControl.show = false
              if (scoreObj.selectedItem) {
                var b = index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp
                if (scoreObj.selectedItem.bowing() === b)
                  b = TnoteItem.BowUndefined
                scoreObj.selectedItem.setBowing(b)
              }
            }
            onVisibleChanged: {
              selected = scoreObj.selectedItem.bowing() === (index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp)
            }
            onEntered: techControl.hideTimer.stop()
            onExited: techControl.active = false
          }
        }
    }
  }
}
