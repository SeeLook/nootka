/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import Score 1.0

ControlBase {
    id: toolbox

    property string rhythmText: NOO.rhythmText(scoreObj.workRhythm)
    property bool triplet: false
    property bool tie: scoreObj.selectedItem && scoreObj.selectedItem.hasTie
    // Accidentals
    property int selectedId: idArray[scoreObj.cursorAlter + 2]
    property string text: selectedId > -1 ? accidGlyphs[selectedId] : ""
    property int alter: accidArray[selectedId + 1]
    readonly property var accidGlyphs: ["\ue264", "\ue260", "\ue262", "\ue263"]
    readonly property var accidArray: [0, -2, -1, 1, 2]
    readonly property var idArray: [0, 1, -1, 2, 3]
    readonly property var rtmActions: [scoreObj.wholeNoteAct, scoreObj.halfNoteAct, scoreObj.quarterNoteAct, scoreObj.eighthNoteAct, scoreObj.sixteenthNoteAct]
    readonly property var accidTips: [qsTr("<b>double flat</b> - lowers a note by two semitones (whole tone).<br>On the guitar it is two frets down."), qsTr("<b>flat</b> - lowers a note by a half tone (semitone).<br>On the guitar it is one fret down."), qsTr("<b>sharp</b> - raises a note by a half tone (semitone).<br>On the guitar it is one fret up."), qsTr("<b>double sharp</b> - raises a note by two semitones (whole tone).<br>On the guitar it is two frets up.")]

    x: show ? 2 : -width - NOO.factor()
    y: score.singleNote || score.meter === Tmeter.NoMeter ? (score.height - height) / 2 : (NOO.isAndroid() ? (nootkaWindow.height - height) / 2 : NOO.factor() / 2)
    z: 1010 // above mobile menu button
    visible: !scoreObj.touched && loader.active
    active: !score.readOnly && (scoreObj.activeNote || (score.noteAdd && score.noteAdd.active))
    factor: NOO.isAndroid() ? NOO.shortScreenSide() * 0.04 : NOO.factor() * 1.2

    Component {
        id: ctrlButtonComp

        ControlButton {
            property int rhythm: 1
            property bool rest: false

            factor: toolbox.factor * 0.9
            yOffset: factor * 0.5
            text: NOO.rhythmText(NOO.rhythm(rhythm, rest, false, false))
            selected: rhythm === scoreObj.workRtmValue && rest === scoreObj.workRtmRest
            onEntered: hideTimer.stop()
            onExited: hideTimer.restart()

            font {
                family: "nootka"
                pixelSize: factor * 2
            }

        }

    }

    Connections {
        target: scoreObj
        onScoreWasCleared: show = false
        onEditModeChanged: show = scoreObj.editMode && scoreObj.activeNote
    }

    component: Component {
        id: contentComp

        Column {
            Grid {
                columns: score.meter === Tmeter.NoMeter ? 1 : 2
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: 4

                    ControlButton {
                        factor: toolbox.factor * 0.9
                        yOffset: factor * -4.4
                        visible: score.enableDoubleAccids || index === 1 || index === 2
                        selected: selectedId === index
                        text: accidGlyphs[index]
                        // HACK: '$' symbol never occurs, so for guitars it is just entire string, when for others just musical part of the text
                        statusTip: scoreObj.riseAct ? accidTips[index].split(GLOB.instrument.isGuitar ? "$" : "<br>")[0] : ""
                        onClicked: scoreObj.cursorAlter = accidArray[(selectedId === index ? -1 : index) + 1]
                        onEntered: hideTimer.stop()
                        onExited: hideTimer.restart()

                        font {
                            family: "scorek"
                            pixelSize: factor * 3
                        }

                    }

                }

            }

            Rectangle {
                visible: score.meter !== Tmeter.NoMeter
                width: toolbox.width
                height: 1
                color: activPal.text
            }

            Grid {
                visible: score.meter !== Tmeter.NoMeter
                columns: 2

                Repeater {
                    model: 10

                    Loader {
                        sourceComponent: ctrlButtonComp
                        onLoaded: {
                            item.rhythm = 1 + index / 2;
                            item.rest = index % 2 === 0;
                            if (index % 2 === 1) {
                                var act = rtmActions[Math.floor(index / 2)];
                                if (act)
                                    item.statusTip = act.text + "<br><b>(" + act.key() + ")</b>";

                            }
                        }

                        Connections {
                            target: item
                            onClicked: {
                                scoreObj.workRtmValue = item.rhythm;
                                scoreObj.workRtmRest = item.rest;
                            }
                        }

                    }

                }
                // triplet

                Loader {
                    //             target: tripLoad.item
                    //             onClicked: { toolbox.triplet = !tripLoad.item.selected; scoreObj.workRhythm = rhythm }
                    //           }

                    id: tripLoad

                    sourceComponent: ctrlButtonComp
                    onLoaded: {
                        item.rhythm = 0;
                        item.text = " ";
                    } //"\u0183"

                    Binding {
                        target: tripLoad.item
                        property: "selected"
                        value: toolbox.triplet
                    }
                    //           Connections {

                }
                // dot

                Loader {
                    id: dotLoad

                    sourceComponent: ctrlButtonComp
                    onLoaded: {
                        item.rhythm = 0;
                        item.text = ".";
                        if (scoreObj.dotNoteAct)
                            item.statusTip = scoreObj.dotNoteAct.text + "<br><b>(" + scoreObj.dotNoteAct.key() + ")</b>";

                    }

                    Binding {
                        target: dotLoad.item
                        property: "selected"
                        value: scoreObj.workRtmDot
                    }

                    Binding {
                        target: dotLoad.item
                        property: "enabled"
                        value: scoreObj.workRtmValue !== Trhythm.Sixteenth
                    }

                    Connections {
                        target: dotLoad.item
                        onClicked: scoreObj.workRtmDot = !scoreObj.workRtmDot
                    }

                }

            }

            // tie
            ControlButton {
                visible: score.meter !== Tmeter.NoMeter
                anchors.horizontalCenter: parent.horizontalCenter
                factor: toolbox.factor
                selected: toolbox.tie
                height: factor * 1.5
                width: factor * 2.7
                yOffset: (height - textHeight) / 2 - factor
                text: "\ue18c"
                onClicked: scoreObj.checkTieOfSelected()
                statusTip: qsTr("Tie - connect or disconnect selected note with previous one if both notes have the same pitch.") + "<br><b>(L)</b>"
                onEntered: hideTimer.stop()
                onExited: hideTimer.restart()

                font {
                    family: "nootka"
                    pixelSize: factor * 6
                }

            }

            Rectangle {
                visible: scoreObj.enableTechnical
                width: toolbox.width
                height: 1
                color: activPal.text
            }

            Grid {
                visible: scoreObj.enableTechnical
                columns: score.meter === Tmeter.NoMeter ? 1 : 2
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: ["\uE610", "\uE612"]

                    ControlButton {
                        factor: toolbox.factor * 0.9
                        yOffset: factor * 0.5
                        text: modelData
                        statusTip: NOO.TR("Bandoneon", index === 0 ? "Bellows is opening." : "Bellows is closing.")
                        onClicked: {
                            show = false;
                            if (scoreObj.selectedItem) {
                                var b = index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp;
                                if (scoreObj.selectedItem.bowing() === b)
                                    b = TnoteItem.BowUndefined;

                                scoreObj.selectedItem.setBowing(b);
                            }
                        }
                        //TODO select bow control according to currently selected note
                        //             onVisibleChanged: selected = scoreObj.selectedItem && scoreObj.selectedItem.bowing() === (index === 0 ? TnoteItem.BowDown : TnoteItem.BowUp)
                        onEntered: hideTimer.stop()
                        onExited: hideTimer.restart()

                        font {
                            family: "nootka"
                            pixelSize: factor * 2
                        }

                    }

                }

            }

        }

    }

    Behavior on x {
        enabled: GLOB.useAnimations

        SpringAnimation {
            spring: 2
            damping: 0.3
            duration: 300
        }

    }

}
