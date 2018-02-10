/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


/**
 * Visibility of this control depends on active (cursor) note of the score
 */
ControlBase {
  id: accidControl

  property int selectedId: idArray[scoreObj.cursorAlter + 2]
  property string text: selectedId > -1 ? accidGlyphs[selectedId] : ""
  property int alter: accidArray[selectedId + 1]
  readonly property var accidGlyphs: [ "\ue264", "\ue260", "\ue262", "\ue263" ]
  readonly property var accidArray: [ 0, -2, -1, 1, 2 ]
  readonly property var idArray: [ 0, 1, -1, 2, 3 ]

  property var techControl: null

  x: (show ? scoreObj.xFirstInActivBar : -Noo.fontSize()) - width
  y: score.contentY + (score.height - height) / 2

  component: Component {
      Column {
        ControlButton {
            factor: accidControl.factor
            height: factor * 2.5
            visible: scoreObj.enableTechnical && scoreObj.selectedItem
            font { pixelSize: factor * 1.5; bold: true }
            text: "⋮"
            onClicked: {
              if (!techControl) {
                var c = Qt.createComponent("qrc:/score/TechnicalControl.qml")
                techControl = c.createObject(score)
              }
              techControl.x = scoreObj.selectedItem.x * score.scale
              techControl.active = true
            }
            onEntered: hideTimer.stop()
            onExited: hideTimer.restart()
          }
          Repeater {
            model: 4
            ControlButton {
              factor: accidControl.factor
              height: factor * 2.5
              yOffset: factor * -4.4
              visible: score.enableDoubleAccids || index === 1 || index === 2
              selected: accidControl.selectedId === index
              font { family: "scorek"; pixelSize: factor * 3 }
              text: accidGlyphs[index]
              onClicked: scoreObj.cursorAlter = accidArray[(selectedId === index ? -1 : index) + 1]
              onEntered: hideTimer.stop()
              onExited: hideTimer.restart()
            }
          }
      }
  }

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}
}
