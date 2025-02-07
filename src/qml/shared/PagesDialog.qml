/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

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
    // private
    property real maxWidth: 0
    property real maxHeight: 0

    function addItem(icon: string, text: string, page: string) : void {
        pages.push("qrc:/" + page + "Page.qml");
        model.append({
            "iconName": "pane/" + icon,
            "buttonText": text
        });
    }

    function selectPage(pageId: int) {
        if (navList.prevButt !== buttons[pageId]) {
            if (typeof (pages[pageId]) === "string")
                pages[pageId] = Qt.createComponent(pages[pageId]).createObject(stack);

            pages[pageId] = stack.replace(pages[pageId]);
            paneFlick.ensureVisible(buttons[pageId].y, buttons[pageId].height, buttons[pageId].x, buttons[pageId].width);
            navList.prevButt = buttons[pageId];
        }
    }

    width: parent.width
    height: parent.height
    columns: topToBott ? 1 : 2
    spacing: 1

    // navigation list on the left or top
    Rectangle {
        // background
        color: NOO.isAndroid() ? "#000000" : "#ffffff"
        height: topToBott ? maxHeight + NOO.factor() / 2 : pagesGrid.height
        width: topToBott ? pagesGrid.width : maxWidth
        z: -1

        Flickable {
            id: paneFlick

            function ensureVisible(yy, hh, xx, ww) {
                if (contentY > yy)
                    contentY = yy;
                else if (contentY + height <= yy + hh)
                    contentY = yy + hh - height;
                if (contentX > xx)
                    contentX = xx;
                else if (contentX + width <= xx + ww)
                    contentX = xx + ww - width;
            }

            clip: true
            anchors.fill: parent
            topMargin: NOO.factor() / 2
            contentWidth: paneGrid.width
            contentHeight: paneGrid.height

            // highlight
            Rectangle {
                width: navList.prevButt?.width
                height: navList.prevButt?.height + NOO.factor() / 4
                color: activPal.highlight
                y: navList.prevButt?.y - NOO.factor() / 8
                x: navList.prevButt ? navList.prevButt.x : 0

                Behavior on y {
                    enabled: GLOB.useAnimations
                    SpringAnimation {
                        spring: 2
                        damping: 0.1
                    }
                }

                Behavior on x {
                    enabled: GLOB.useAnimations
                    SpringAnimation {
                        spring: 2
                        damping: 0.1
                    }
                }
            }

            Grid {
                id: paneGrid

                spacing: NOO.factor() / 4
                columns: topToBott ? model.count : 1

                Repeater {
                    id: navList

                    property PaneButton prevButt: null

                    model: ListModel {
                        id: pageModel
                    }

                    delegate: PaneButton {
                        id: delegateButt

                        name: buttonText
                        pixmap: NOO.pix(iconName)
                        onClicked: selectPage(index)
                        Component.onCompleted: {
                            // keep buttons width the same

                            maxWidth = Math.max(maxWidth, width);
                            maxHeight = Math.max(maxHeight, height);
                            buttons.push(delegateButt);
                            for (var i = 0; i < buttons.length; ++i) buttons[i].width = maxWidth
                            if (index === 0) {
                                navList.prevButt = delegateButt;
                                pages[0] = stack.push(Qt.createComponent(pages[0]).createObject(stack));
                            }
                        }
                    }

                }

            }

            ScrollBar.vertical: ScrollBar {
                visible: !topToBott
                active: false
            }

            ScrollBar.horizontal: ScrollBar {
                visible: topToBott
                active: false
            }

        }

    }

    // pages container on the right or bottom
    StackView {
        id: stack

        clip: true
        width: pagesGrid.width - (topToBott ? 0 : maxWidth) - pagesGrid.spacing
        height: pagesGrid.height - (topToBott ? maxHeight + NOO.factor() / 2 : 0)

        replaceEnter: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "x"
                from: width
                to: 0
            }
        }

        replaceExit: Transition {
            enabled: GLOB.useAnimations

            NumberAnimation {
                property: "x"
                from: 0
                to: -width
            }
        }

    }

}
