/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Main 1.0


/**
 * To get this, invoke:
 * mootka --nootini
 */
TaudioAnalyzeItem {
  id: aa
  width: parent.width; height: parent.height / 2; y: parent.height - height
  z: 100

  property real sc: 1.0

  // private
  property var noteRects: []

  Rectangle {
    anchors.fill: parent
    color: activPal.base
    MouseArea {
      anchors.fill: parent
      onWheel: {
        if (wheel.modifiers & Qt.ControlModifier) {
            if (wheel.angleDelta.y > 0)
              sc *= 1.0625
            else if (wheel.angleDelta.y < 0)
              sc *= 0.9375
        } else
            wheel.accepted = false
      }
    }
  }

  ListModel { id: audioModel }

  onDataReady: {
    audioModel.append( { "vol": pcmVol, "energy": energy, "onSet": onSet } )
  }

  Shortcut {
    sequence: "Shift+backspace"
    onActivated: {
      tip.nr = -1
      audioModel.clear()
      for (var n = 0; n < noteRects.length; ++n)
        noteRects[n].destroy()
      noteRects.length = 0
    }
  }

  ListView {
    id: lv
    anchors.fill: parent
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds

    model: audioModel

    delegate: Rectangle {
      width: 5 * aa.sc; height: aa.height
      color: onSet ? "orange" : (ma.containsMouse ? "#11111111" : (vol > 0 ? "transparent" : "#110000ff"))

      Rectangle { width: parent.width; height: (aa.height / 2) * energy; color: "#55ff0000"; y: parent.height / 2 - height }
      Rectangle { width: parent.width; height: width; radius: width / 2; color: "blue"; y: parent.height * (1.0 - vol) }
      MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true;
        onEntered: tip.nr = index
        onExited: tip.nr = -1
      }
    }
    Rectangle { width: parent.width; height: 1; color: activPal.text; y: parent.height / 2 }
  }

  Component {
    id: noteComp
    Rectangle {
      property alias name: noteName.text
      height: aa.height / 6; y: height / 2
      color: "transparent"
      border { color: name === "\ue4e5" ? activPal.highlight : activPal.text; width: 1 }
      Text {
        id: noteName
        color: activPal.text
        font { family: "Scorek"; pixelSize: Noo.fontSize() * 2 }
        textFormat: Text.StyledText
        x: (parent.width - width) / 2; y: height * -0.2
      }
    }
  }

  Rectangle {
    id: tip
    property int nr: -1
    property var mD: audioModel.get(nr)
    visible: nr > -1
    border { color: activPal.text; width: 1 }
    radius: Noo.fontSize()
    width: Noo.fontSize() * 40; height: contText.height + Noo.fontSize()
    x: (parent.width - width) / 2; y: -height - Noo.fontSize()
    Text {
      id: contText
      anchors.centerIn: parent
      color: activPal.text
      text: tip.mD ? tip.nr + "\nvolume: " + tip.mD.vol + "\ndynamic: " + tip.mD.energy: ""
    }
  }

  onNoteData: {
    noteRects.push(noteComp.createObject(lv.contentItem,
                          {
                            "width": Qt.binding(function() { return (end - start + 1) * 5 * aa.sc }),
                            "x": Qt.binding(function() { return start * 5 * aa.sc }),
                            "name": note
                          })
              )
  }

}
