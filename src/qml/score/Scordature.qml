/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick
import Nootka.Music
import Score 1.0

Grid {
    property real realHeight: Math.floor(scordModel.count / columns) * 12 * scale

    y: upperLine + 14 + (score.clef === Tclef.PianoStaffClefs ? 18 : 0)
    x: 0.5
    spacing: 0.5
    scale: columns > 1 ? 0.2 : (score.clef === Tclef.PianoStaffClefs ? 0.35 : 0.3)
    transformOrigin: Item.TopLeft
    columns: GLOB.tuning.changedStrings() > 3 ? 2 : 1
    visible: score.clef !== Tclef.NoClef
    Component.onCompleted: {
        for (var s = 0; s < 6; ++s) {
            if (GLOB.tuning.otherThanStd(s + 1))
                scordModel.append({
                "strNr": s + 1,
                "name": GLOB.tuning.stringName(s + 1)
            });

        }
    }

    Repeater {
        Row {
            height: 12

            Text {
                text: strNr
                anchors.verticalCenter: parent.verticalCenter

                font {
                    pixelSize: 12
                    family: "Nootka"
                }

            }

            Text {
                topPadding: 3.5
                text: "=" + name
                anchors.verticalCenter: parent.verticalCenter

                font {
                    pixelSize: 8
                    family: "Scorek"
                }

            }

        }

        model: ListModel {
            id: scordModel
        }

    }

    Connections {
        target: GLOB
        onNoteNameStyleChanged: {
            for (var s = 0; s < scordModel.count; ++s) scordModel.get(s).name = GLOB.tuning.stringName(scordModel.get(s).strNr)
        }
    }

}
