/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ComboBox {
  id: cb

  property alias radius: bg.radius

  height: Noo.fontSize() * 2
  font.pixelSize: Noo.fontSize()

  scale: GLOB.useAnimations && cb.pressed ? 0.9 : 1.0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  /**
   * locks/unlocks @p itemNr among combo box delegates depending on @p state:
   * @p true - lock; @p false - unlock
   */
  function lock(itemNr, state) {
    if (itemNr < lockList.length)
      lockList[itemNr] = state
    else
      console.log("[TcomboBox] not such an item. FixMe!", itemNr)
  }

  // private
  property var lockList: []

  onCountChanged: {
    var loops = Math.max(count, lockList.length)
    for (var i = 0; i < loops; i++) {
      if (i < lockList.length) {
        if (lockList.length < count)
          lockList.splice(i, 1)
        else
          lockList[i] = false
      } else {
          if (count > lockList.length)
            lockList.push(false)
      }
    }
  }

  delegate: ItemDelegate {
    id: itDel
    width: cb.width; height: Noo.fontSize() * 2.5
    hoverEnabled: true
    enabled: !lockList[index]
    background: Rectangle {
      color: cb.currentIndex === index || itDel.down ? activPal.highlight : (itDel.hovered ? Noo.alpha(activPal.highlight, 70) : "transparent")
    }
    contentItem: Text {
      text: modelData
      color: itDel.enabled ? (cb.currentIndex === index || itDel.down ? activPal.highlightedText : activPal.text) : disdPal.text
      textFormat: Text.StyledText
      verticalAlignment: Text.AlignVCenter
      scale: GLOB.useAnimations && itDel.pressed ? 0.9 : 1.0
      font { pixelSize: cb.font.pixelSize; strikeout: !itDel.enabled; bold: cb.currentIndex === index }
      Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    }
    highlighted: cb.highlightedIndex === index
  }

  indicator: Text {
    x: cb.width - width * (Noo.isAndroid() || Noo.isMac() ? 1.2 : 1)
    color: cb.enabled ? activPal.text : disdPal.text; text: "⋮"
    font { pixelSize: cb.height * 0.7; bold: true }
    anchors.verticalCenter: parent.verticalCenter
  }

  contentItem: Text {
    font: cb.font; leftPadding: cb.height / 3
    text: displayText; color: cb.enabled ? activPal.text : disdPal.text
    verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
  }

  background: TipRect {
    id: bg
    color: cb.enabled ? activPal.button : disdPal.button; radius: 0
    rised: !cb.pressed
  }

  popup: Popup {
    parent: cb.parent
    x: cb.x; y: cb.y + cb.height + Noo.fontSize() / 4
    scale: GLOB.useAnimations ? 0.1 : 1.0
    padding: 0

    width: cb.width
    implicitHeight: contentItem.implicitHeight
    transformOrigin: Item.Top

    contentItem: ListView {
      clip: true
      implicitHeight: Math.min(contentHeight, Noo.fontSize() * 15) // 6 items
      model: cb.popup.visible ? cb.delegateModel : null
      currentIndex: cb.highlightedIndex

      ScrollBar.vertical: ScrollBar { active: cb.delegateModel.count > 6 }
    }

    background: TipRect { shadowRadius: Noo.fontSize(); color: activPal.window }
    enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1 }}
    exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}
  }
}
