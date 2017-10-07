/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Text {
  id: clef

  property int type: Tclef.Treble_G
  property var clefDrawer

  width: 5.5
  x: 0.5
  y: 5
  text: "\ue050" // treble clef by default
  font { family: "Scorek"; pixelSize: 8 }
  color: activPal.text

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerClef : null }
  Component {
    id: lowerClef
    Text { // bass clef at lower staff
      font: clef.font
      text: "\ue062"
      color: clef.color
      y: clef.y + 7
    }
  }

  Component {
    id: clefComp
    Drawer {
      id: drawer
      property alias selectedClef: clefMenu.selClef
      width: Noo.fontSize() * 18
      height: nootkaWindow.height
      visible: true
      Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: clefMenu.height
        ClefMenu {
          id: clefMenu
          onClicked: { type = cl; close() }
          onSelIdChanged: { // ensure visible
            var yy = Noo.fontSize() * 7.75 * selId
            if (flick.contentY >= yy)
              flick.contentY = yy
            else if (flick.contentY + flick.height <= yy + Noo.fontSize() * 7.75)
              flick.contentY = yy + Noo.fontSize() * 7.75 - flick.height
          }
        }
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    enabled: !score.readOnly
    onClicked: {
      if (clefDrawer)
        clefDrawer.open()
      else
        clefDrawer = clefComp.createObject(clef)
      clefDrawer.selectedClef = type
    }
  }

  onTypeChanged: {
      x = 0.5
      switch (clef.type) {
        case Tclef.Treble_G:
        case Tclef.Treble_G_8down:
          y = 5; break;
        case Tclef.Bass_F:
        case Tclef.Bass_F_8down:
        case Tclef.Tenor_C:
          y = 1; break;
        case Tclef.Alto_C:
          y = 3; break;
        case Tclef.PianoStaffClefs:
          y = 3; x = 2.5; break;
        case Tclef.NoClef:
          y = 3; x = 2; break;
      }
      text = Noo.clef(clef.type).glyph()
  }
}




