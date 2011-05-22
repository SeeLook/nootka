/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tscorewidget.h"
#include "tnoteview.h"
#include "tglobals.h"
#include "tkeysignature.h"
//#include <QDebug>


extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent)
{


    noteViews[0]->setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals."));

    if (gl->enharmNotesColor == -1)
        gl->enharmNotesColor = palette().highlight().color();

    noteViews[1]->setDisabled(true);
    noteViews[1]->setColor(gl->enharmNotesColor);
    noteViews[2]->setDisabled(true);
    noteViews[2]->setColor(gl->enharmNotesColor);

    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setEnableKeySign(gl->SkeySignatureEnabled);

    setAmbitus(Tnote(gl->Gtune.lowest().getChromaticNrOfNote()-1),
               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber+1));

    isExamExecuting(false);
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
            clearNote(1);
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                setNote(2,*(it));
            else
                clearNote(2);
        }
    }
    emit noteChanged(index, note);
}

void TscoreWidget::setEnableEnharmNotes(bool isEnabled) {
    if (!isEnabled) {
        clearNote(1);
        clearNote(2);
    }
}

void TscoreWidget::paintEvent(QPaintEvent *event) {
  TscoreWidgetSimple::paintEvent(event);
  if (gl->Gtune != Ttune::stdTune) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0,0,width(),height());
    painter.setPen(QPen(palette().foreground().color()));
//    I HATE THIS PART... BUT IT WORKS
    /** @todo Its size has to depends on @param coeff */
    QFont f = QFont("Arial");
    f.setPixelSize(11);
    painter.setFont(f);
    Ttune sT = Ttune::stdTune;
    int nL = 0;
    for (int i=1; i<7; i++) {
        if ( gl->Gtune[i] != sT[i])
            nL++;
    }
    int xOffBase = coeff;
    if (nL>3) xOffBase = 0;
    int xOff = xOffBase;
    int yOff = -1;
    int c = 0;
    for (int i=1; i<7; i++) {
        if ( gl->Gtune[i] != sT[i]) {
        if ( nL>3 && c%2 == 1 ) {
            xOff = 45;
        } else {
            yOff++;
            xOff = xOffBase;
        }
        c++;
        int fa =15;
        painter.drawText(QRectF(5+xOff,29*coeff+17*yOff, fa, fa), Qt::AlignCenter,
               QString("%1").arg(i));
        painter.drawText(QRectF(5+xOff+fa, 29*coeff+17*yOff, 60, fa),Qt::AlignLeft,
             " =" + QString::fromStdString(gl->Gtune[i].getName(gl->NnameStyleInNoteName,false)));
        painter.drawEllipse(5+xOff,29*coeff+17*yOff,fa,fa);
      }
    }
  }
}

void TscoreWidget::acceptSettings() {
    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    if (gl->Gtune != Ttune::stdTune)
        setHasScord(true);
    else // resizing is done in setEnableKeySign() method;
        setHasScord(false);
    setEnableKeySign(gl->SkeySignatureEnabled);
    if (!gl->doubleAccidentalsEnabled) clearNote(2);
    setEnableEnharmNotes(gl->showEnharmNotes);
    if (gl->SkeySignatureEnabled) refreshKeySignNameStyle();
    setAmbitus(Tnote(gl->Gtune.lowest().getChromaticNrOfNote()-1),
               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber+1));
    update();
}

void TscoreWidget::askQuestion(Tnote note, char realStr) {
    setNote(1, note);
    if (realStr) 
      noteViews[1]->setString(realStr);
}

void TscoreWidget::askQuestion(Tnote note, TkeySignature key, char realStr) {
    setKeySignature(key);
    askQuestion(note, realStr);
}

void TscoreWidget::clearScore() {
  clearNote(0);
  clearNote(1);
  noteViews[1]->removeString(); // so far string number to remove occur only on this view
  clearNote(2); // also hide question mark when will be implemented
  setKeySignature(TkeySignature());
}

void TscoreWidget::isExamExecuting(bool isIt) {
    if (isIt)
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
    else
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
}

void TscoreWidget::unLockScore() {
    setScoreDisabled(false);
    noteViews[1]->setDisabled(true);
    noteViews[2]->setDisabled(true);
}


