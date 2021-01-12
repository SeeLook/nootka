/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Dialogs 1.0
import Score 1.0
import "../score"

MouseArea {
  id: wrapArea

  height: Noo.fontSize() * 10

  property bool held: false
  property alias nr: wrapper.nr

  function updateMelody() { wrapper.updateMelody() }

  hoverEnabled: true
  drag.target: wrapArea.held ? wrapper : undefined
  drag.axis: Drag.YAxis

  onPressAndHold: wrapArea.held = true
  onReleased: wrapArea.held = false
  onClicked: melListView.currentMelody = nr

  cursorShape: held ? Qt.SizeVerCursor : (containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor)

  TmelodyWrapper {
    id: wrapper

    score: sc.scoreObj
    melodyView: melListView

    width: wrapArea.width; height: wrapArea.height

    Drag.active: wrapArea.held
    Drag.source: wrapArea
    Drag.hotSpot.x: width / 2
    Drag.hotSpot.y: height / 2

    anchors {
      horizontalCenter: parent.horizontalCenter
      verticalCenter: parent.verticalCenter
    }

    states: State {
      when: wrapArea.held
      ParentChange { target: wrapper; parent: melListView.viewRoot }
      AnchorChanges {
        target: wrapper
        anchors { horizontalCenter: undefined; verticalCenter: undefined }
      }
    }

    Score {
      id: sc
      anchors.fill: parent
      interactive: false
      readOnly: true
      bgColor: Qt.tint(activPal.base, Noo.alpha(wrapArea.held ? activPal.text : (nr === melListView.currentMelody ? activPal.highlight : activPal.base), 50))
      Behavior on bgColor { enabled: GLOB.useAnimations; ColorAnimation { duration: 150 } }
    }

    Rectangle { // this is part of covering rectangle
      height: parent.height; width: parent.height
      rotation: -90;
      x: parent.width - scoreCover.width - width
      gradient: Gradient {
        GradientStop { position: 0.0; color: "transparent" }
        GradientStop { position: 0.75; color: sc.bgColor }
      }
    }
    Rectangle {
      id: scoreCover
      height: parent.height; width: parent.width - parent.height * 4
      anchors { right: parent.right }
      color: sc.bgColor
    }

    Text {
      x: Noo.fontSize() / 2; y: Noo.fontSize() / 2
      font { family: "Sans"; bold: true }
      text: nr + 1; color: Noo.alpha(activPal.text, 150)
    }

    Text {
      text: wrapper.title
      x: parent.width - scoreCover.width
      y: Noo.fontSize() / 4
      font { bold: true; pixelSize: Noo.fontSize() * 1.3 }
      color: wrapArea.containsMouse ? activPal.text : Noo.alpha(activPal.text, 150)
      width: Noo.fontSize() * 25; elide: Text.ElideRight
      Behavior on color { enabled: GLOB.useAnimations; ColorAnimation {} }
    }

    Text {
      text: wrapper.composer
      anchors { right: parent.right; rightMargin: Noo.fontSize() / 4 }
      y: Noo.fontSize() * 1.5
      color: wrapArea.containsMouse ? activPal.text : Noo.alpha(activPal.text, 150)
      maximumLineCount: 1
      Behavior on color { enabled: GLOB.useAnimations; ColorAnimation {} }
    }
  } // TmelodyWrapper

  DropArea {
    anchors { fill: parent; margins: 10 }
    onEntered: {
      if (drag.source.nr !== wrapArea.nr) {
        if (drag.source.nr === melListView.currentMelody)
          melListView.currentMelody = wrapper.nr
        melListView.moveMelody(drag.source.nr, wrapArea.nr)
      }
    }
  }
}
