/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


/**
 * Implements a dialog with navigation list on the left
 * Model has following fields:
 * 'icon', 'text' and 'page' - with path to QML file
 */
Grid {
  id: pagesGrid

  property bool topToBott: parent.height > parent.width
  property alias stack: stack
  property alias model: navList.model
  property var pages: []
  property alias currentPage: stack.currentItem
  property var buttons: []

  width: parent.width; height: parent.height

  function addItem(icon, text, page) {
    pages.push("qrc:/" + page + "Page.qml")
    model.append({ "iconName": "pane/" + icon, "buttonText": text })
  }

  columns: topToBott ? 1 : 2

  spacing: 1

  // private
  property real maxWidth: 0
  property real maxHeight: 0

  // navigation list on the left or top
  Rectangle { // background
    color: NOO.isAndroid() ? "#000000" : "#ffffff"
    height: topToBott ? maxHeight + NOO.factor() / 2 : pagesGrid.height
    width: topToBott ? pagesGrid.width : maxWidth
    z: -1

    Flickable {
      id: paneFlick
      clip: true
      anchors.fill: parent
      topMargin: NOO.factor() / 2
      contentWidth: paneGrid.width; contentHeight: paneGrid.height

      Rectangle { // highlight
        width: navList.prevButt.width; height: navList.prevButt.height + NOO.factor() / 4
        color: activPal.highlight
        y: navList.prevButt.y - NOO.factor() / 8; x: navList.prevButt.x
        Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1 }}
        Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1 }}
      }

      Grid {
        id: paneGrid
        spacing: NOO.factor() / 4
        columns: topToBott ? model.count : 1

        Repeater {
          id: navList
          property PaneButton prevButt: null
          property int prevDelegate: -1

          model: ListModel { id: pageModel }

          delegate: PaneButton {
            id: delegateButt
            name: buttonText
            pixmap: NOO.pix(iconName)
            onClicked: {
              if (navList.prevButt !== delegateButt) {
                if (typeof(pages[index]) === "string")
                  pages[index] = Qt.createComponent(pages[index]).createObject(stack)
                pages[index] = stack.replace(pages[index])
                paneFlick.ensureVisible(y, height, x, width)
                navList.prevButt = delegateButt
              }
            }
            Component.onCompleted: {
              navList.prevDelegate = index
              maxWidth = Math.max(maxWidth, width)
              maxHeight = Math.max(maxHeight, height)
              buttons.push(delegateButt)
              for (var i = 0; i < buttons.length; ++i) // keep buttons width the same
                buttons[i].width = maxWidth
              if (index === 0) {
                navList.prevButt = delegateButt
                pages[0] = stack.push(Qt.createComponent(pages[0]).createObject(stack))
              }
            }
          }

        }
      }

      ScrollBar.vertical: ScrollBar { visible: !topToBott; active: false }
      ScrollBar.horizontal: ScrollBar { visible: topToBott; active: false }

      function ensureVisible(yy, hh, xx, ww) {
        if (contentY > yy)
          contentY = yy
        else if (contentY + height <= yy + hh)
          contentY = yy + hh - height
        if (contentX > xx)
          contentX = xx
        else if (contentX + width <= xx + ww)
          contentX = xx + ww - width
      }
    }
  }

  // pages container on the right or bottom
  StackView {
    id: stack
    clip: true
    width: pagesGrid.width - (topToBott ? 0 : maxWidth) - pagesGrid.spacing
    height: pagesGrid.height - (topToBott ? maxHeight + NOO.factor() / 2 : 0)
    replaceEnter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "x"; from: width; to: 0 }}
    replaceExit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "x"; from: 0; to: -width }}
  }
}

