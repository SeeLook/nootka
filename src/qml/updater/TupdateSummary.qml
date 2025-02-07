/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import Nootka.Update 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TupdateItem {
    id: updateIt

    property string titleText: NOO.isAndroid() ? "" : qsTr("Updates")

    function accepted() {
        dialLoader.close();
    }

    width: parent.width
    height: parent.height

    Connections {
        target: dialogObj
        onUpdateSummary: {
            console.log(version);
            updateIt.version = version;
            updateIt.changes = changes;
        }
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1)

        Item {
            width: height
            height: NOO.isAndroid() ? 0 : NOO.factor()
        }

        LinkText {
            id: headText

            visible: onlineIsNewer
            width: updateIt.width - NOO.factor()
            text: onlineIsNewer ? qsTr("New Nootka %1 is available.").arg(version) + "<br>" + qsTr("To get it, visit %1 Nootka site").arg("<a href=\"https://nootka.sourceforge.io/index.php/download/\">") + "</a>." : ""
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter

            font {
                pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.5)
                bold: true
            }

        }

        GlowRect {
            width: updateIt.width - NOO.factor() * 2
            height: updateIt.height - upCol.height - (onlineIsNewer ? headText.height : 0) - NOO.factor() * (NOO.isAndroid() ? 1 : 4)
            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.base

            Tflickable {
                anchors.fill: parent
                contentWidth: width
                contentHeight: chText.height + NOO.factor()

                LinkText {
                    id: chText

                    padding: NOO.factor() / 2
                    width: updateIt.width - NOO.factor() * 2
                    textFormat: Text.StyledText
                    text: changes
                    wrapMode: Text.WordWrap
                    horizontalAlignment: onlineIsNewer ? Text.AlignLeft : Text.AlignHCenter
                }

            }

        }

        Column {
            id: upCol

            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

            TcheckBox {
                id: checkChB

                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTranslate("TupdateRulesWdg", "check for Nootka updates")
                checked: updateCheck
                onToggled: updateCheck = checked
            }

            Tile {
                enabled: checkChB.checked
                width: checkRow.width + NOO.factor() * 2

                Row {
                    id: checkRow

                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: NOO.factor() * (NOO.isAndroid() ? 2 : 4)

                    ButtonGroup {
                        buttons: periodCol.children
                    }

                    Column {
                        id: periodCol

                        spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                        TcheckBox {
                            id: dailyChB

                            text: qsTranslate("TupdateRulesWdg", "daily")
                            checked: rules.width === 0
                            onToggled: rules = Qt.size(0, rules.height)
                        }

                        TcheckBox {
                            id: weeklyChB

                            text: qsTranslate("TupdateRulesWdg", "weekly")
                            checked: rules.width === 1
                            onToggled: rules = Qt.size(1, rules.height)
                        }

                        TcheckBox {
                            id: monthlyChB

                            text: qsTranslate("TupdateRulesWdg", "monthly")
                            checked: rules.width > 1
                            onToggled: rules = Qt.size(2, rules.height)
                        }

                    }

                    ButtonGroup {
                        buttons: versionCol.children
                    }

                    Column {
                        id: versionCol

                        anchors.verticalCenter: parent.verticalCenter
                        spacing: NOO.factor() * (NOO.isAndroid() ? 0.5 : 1)

                        TcheckBox {
                            id: allChB

                            text: qsTranslate("TupdateRulesWdg", "all new versions")
                            checked: rules.height === 0
                            onToggled: rules = Qt.size(rules.width, 0)
                        }

                        TcheckBox {
                            id: stableChB

                            text: qsTranslate("TupdateRulesWdg", "stable versions only")
                            checked: rules.height !== 0
                            onToggled: rules = Qt.size(rules.width, 1)
                        }

                    }

                }

            }

        }

    }

}
