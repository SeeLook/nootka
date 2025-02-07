/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

ListView {
    id: meterList

    property int meters: 0

    signal metersModified()

    clip: true
    width: model * (NOO.factor() * 4.5)
    contentWidth: model * (NOO.factor() * 4.25)
    height: NOO.factor() * 3
    orientation: ListView.Horizontal
    spacing: NOO.factor() / 4
    model: 12

    ScrollBar.horizontal: ScrollBar {
        active: enabled
        visible: active
    }

    delegate: Component {
        Rectangle {
            property int mask: Math.pow(2, index)

            width: NOO.factor() * 5
            height: NOO.factor() * 3
            color: enabled ? (index % 2 ? activPal.alternateBase : activPal.base) : Qt.darker(disdPal.window, 1.2)

            TcheckBox {
                width: NOO.factor() * 5
                x: -y
                y: NOO.factor() * 0.2
                checked: meters & mask
                onClicked: {
                    if (checked)
                        meters |= mask;
                    else
                        meters &= ~mask;
                    metersModified();
                }

                contentItem: Item {
                    height: NOO.factor() * 2
                    width: NOO.factor() * 2

                    Text {
                        y: NOO.factor() * -3
                        text: NOO.meter(Math.pow(2, index)).symbol()
                        leftPadding: NOO.factor() * 2.5
                        color: enabled ? activPal.text : disdPal.text

                        font {
                            family: "Scorek"
                            pixelSize: NOO.factor() * 2.2
                        }

                    }

                }

            }

        }

    }

}
