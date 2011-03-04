/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "tscorewidget.h"
#include "tnoteview.h"
//#include "scoresetttingsdlg.h"
#include "tglobals.h"
#include <QMenu>

#include <iostream>

extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent)
{


    noteViews[0]->setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals. Right click for more"));

    if (gl->enharmNotesColor == -1) { // enharmonic notes will have inverted highlight color
        QColor hi;
        hi = palette().highlight().color();
        hi.setRgb(qRgb(255-hi.red(),255-hi.green(),255-hi.blue()));
        gl->enharmNotesColor = hi;
    }
    noteViews[1]->setDisabled(true);
    noteViews[1]->setColor(gl->enharmNotesColor);
    noteViews[2]->setDisabled(true);
    noteViews[2]->setColor(gl->enharmNotesColor);

//    contextMenu = new QMenu(this);

    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setEnableKeySign(gl->keySignatureEnabled);

//    createActions();

    connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
}


void TscoreWidget::whenNoteWasChanged(int index, Tnote note) {
    //We are sure that index is 0, cause others are disabled :-)
    if (gl->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            setNote(1,*(it));
        else
            hideNote(1);
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                setNote(2,*(it));
            else
                hideNote(2);
        }
//        if (enharmList.size())
//          for (int i = 1; i<enharmList.size(); i++) {
//            if (i == 1) {
//                setNote(1,enharmList[1]);
//                if (enharmList.size() == 1)
//                    hideNote(2);
//            }
//            if (i == 2) setNote(2,enharmList[2]);
//          }
    }
}

void TscoreWidget::setEnableEnharmNotes(bool isEnabled) {
    if (!isEnabled) {
        noteViews[1]->hideNote();
        noteViews[2]->hideNote();
    }
}

//void TscoreWidget::contextMenuEvent(QContextMenuEvent *event) {
//    if (gl->keySignatureEnabled)
//        contextMenu->addAction(keySigNameAct);
//    else contextMenu->removeAction(keySigNameAct);
//    contextMenu->addAction(scoreSettAct);
//    contextMenu->exec(event->globalPos());

//}

//void TscoreWidget::createActions() {
//    keySigNameAct = new QAction(ScoreSettingsDlg::showKeySigName,this);
//    keySigNameAct->setCheckable(true);
//    keySigNameAct->setChecked(gl->showKeySignName);
//    connect(keySigNameAct, SIGNAL(triggered(bool)), this, SLOT(enableKeySigNameSlot(bool)));

//    scoreSettAct = new QAction(tr("score settings"),this);
//    connect(scoreSettAct, SIGNAL(triggered()), this, SLOT(createScoreSettingsDialog()));
//}


//void TscoreWidget::createScoreSettingsDialog() {

//    ScoreSettingsDlg *scoreSettDlg = new ScoreSettingsDlg();
//    if (scoreSettDlg->exec() == QDialog::Accepted) {
//        keySigNameAct->setChecked(gl->showKeySignName);
//        enableDblAccidsSlot(gl->doubleAccidentalsEnabled);
//        emit enableDblAccWasChanged(gl->doubleAccidentalsEnabled);
//        setEnableKeySign(gl->keySignatureEnabled);
//        if (gl->keySignatureEnabled) refreshKeySignNameStyle();
//    }
//}

//void TscoreWidget::enableDblAccidsSlot(bool isEnabled) {
//    setEnabledDblAccid(isEnabled);
//    gl->doubleAccidentalsEnabled = isEnabled;
//}

//void TscoreWidget::enableKeySigNameSlot(bool isEnabled) {
//    gl->showKeySignName = isEnabled;
//    refreshKeySignNameStyle();
//}
