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
#include "scoresetttingsdlg.h"
#include "tglobals.h"
#include <QMenu>

#include <iostream>

extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent)
{


    noteViews[0]->setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals. Right click for more"));
    noteViews[1]->setDisabled(true);
    noteViews[2]->setDisabled(true);

    contextMenu = new QMenu(this);

    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setEnableKeySign(gl->keySignatureEnabled);

    createActions();

    connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
}

void TscoreWidget::createActions() {
    keySigNameAct = new QAction(ScoreSettingsDlg::showKeySigName,this);
    keySigNameAct->setCheckable(true);
    keySigNameAct->setChecked(gl->showKeySignName);
    connect(keySigNameAct, SIGNAL(triggered(bool)), this, SLOT(enableKeySigNameSlot(bool)));

    scoreSettAct = new QAction(tr("score settings"),this);
    connect(scoreSettAct, SIGNAL(triggered()), this, SLOT(createScoreSettingsDialog()));
}

void TscoreWidget::contextMenuEvent(QContextMenuEvent *event) {
    if (gl->keySignatureEnabled)
        contextMenu->addAction(keySigNameAct);
    else contextMenu->removeAction(keySigNameAct);
    contextMenu->addAction(scoreSettAct);
    contextMenu->exec(event->globalPos());

}

void TscoreWidget::createScoreSettingsDialog() {

    ScoreSettingsDlg *scoreSettDlg = new ScoreSettingsDlg();
    if (scoreSettDlg->exec() == QDialog::Accepted) {
        keySigNameAct->setChecked(gl->showKeySignName);
        enableDblAccidsSlot(gl->doubleAccidentalsEnabled);
        emit enableDblAccWasChanged(gl->doubleAccidentalsEnabled);
        setEnableKeySign(gl->keySignatureEnabled);
        if (gl->keySignatureEnabled) refreshKeySignNameStyle();
    }
}

void TscoreWidget::enableDblAccidsSlot(bool isEnabled) {
    setEnabledDblAccid(isEnabled);
    gl->doubleAccidentalsEnabled = isEnabled;
}

void TscoreWidget::enableKeySigNameSlot(bool isEnabled) {
    gl->showKeySignName = isEnabled;
    refreshKeySignNameStyle();
}

void TscoreWidget::whenNoteWasChanged(int index, Tnote note) {
    //We are sure that index is 0, cause others are disabled :-)
    std::cout << note.getName(gl->nameStyleInKeySign,true).toStdString() << "\n";
    if (gl->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        QList<Tnote>::iterator it = enharmList.begin();
        ++it; //move one position, cause first is the main note
        if (it != enharmList.end()) {
            setNote(1,*(it));
//            std::cout << get .getName(gl->nameStyleInKeySign,true).toStdString() << "\n";
        }
//        else
//            hide
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end()) {
                setNote(2,*(it));
//                std::cout << *(it).getName(gl->nameStyleInKeySign,true).toStdString() << "\n";
            }
//            else hide
        }
    }

}
