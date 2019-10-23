/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ComboBox {
  id: cb

  height: Noo.fontSize() * 2
  scale: GLOB.useAnimations && cb.pressed ? 0.9 : 1.0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  
  delegate: ItemDelegate {
    id: itDel
    width: cb.width
    background: Rectangle {
      color: itDel.pressed ? activPal.highlight : (itDel.highlighted ? Noo.alpha(activPal.highlight, 50) : "transparent")
    }
    contentItem: Text {
      text: modelData
      color: activPal.text; //font: cb.font
      elide: Text.ElideRight
      verticalAlignment: Text.AlignVCenter
      scale: GLOB.useAnimations && itDel.pressed ? 0.9 : 1.0
      Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    }
    highlighted: cb.highlightedIndex === index
  }
  
  indicator: Text {
    x: cb.width - width
    color: activPal.text; text: "⋮"; font.pixelSize: cb.height * 0.6
    anchors.verticalCenter: parent.verticalCenter
  }
  
  contentItem: Text {
    font: cb.font; leftPadding: cb.height / 3
    text: displayText; color: activPal.text
    verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
  }
  
  background: TipRect {
    color: Qt.lighter(activPal.window, 1.2); radius: 0
    rised: !cb.pressed
  }
  
  popup: Tmenu {
    y: cb.height - 1
    width: cb.width
    implicitHeight: contentItem.implicitHeight
    padding: 1
    transformOrigin: Item.Top
    
    contentItem: ListView {
      clip: true
      implicitHeight: contentHeight
      model: cb.popup.visible ? cb.delegateModel : null
      currentIndex: cb.highlightedIndex
      
      ScrollIndicator.vertical: ScrollIndicator { }
    }
    
    background: TipRect { shadowRadius: Noo.fontSize(); color: Qt.lighter(activPal.window, 1.2) }
  }
}
