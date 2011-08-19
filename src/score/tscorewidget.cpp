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
#include "tkeysignatureview.h"
#include "tglobals.h"
#include "tkeysignature.h"
#include <QtGui>


extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent)
{


    noteViews[0]->setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals."));

    if (gl->enharmNotesColor == -1)
        gl->enharmNotesColor = palette().highlight().color();
    if (gl->SpointerColor == -1) {
        gl->SpointerColor = gl->invertColor(palette().highlight().color());
        gl->SpointerColor.setAlpha(200);
    }

    noteViews[0]->setPointedColor(gl->SpointerColor);
    noteViews[1]->setDisabled(true);
    noteViews[1]->setColor(gl->enharmNotesColor);
    noteViews[2]->setDisabled(true);
    noteViews[2]->setColor(gl->enharmNotesColor);

    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setEnableKeySign(gl->SkeySignatureEnabled);
    
    m_questMark = 0;
    m_questKey = 0;
    setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
               Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

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

void TscoreWidget::resizeEvent(QResizeEvent *event) {
    TscoreWidgetSimple::resizeEvent(event);
    if (m_questMark) {
        resizeQuestMark();
    }
    if (m_questKey && m_questKey->isVisible())
        resizeKeyText();
}

void TscoreWidget::paintEvent(QPaintEvent *event) {
  TscoreWidgetSimple::paintEvent(event);
  if (gl->Gtune() != Ttune::stdTune) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0,0,width(),height());
    painter.setPen(QPen(palette().foreground().color()));
//    I HATE THIS PART... BUT IT WORKS
    /** @todo Its size has to depends on @param coeff */
    QFont f = QFont("Arial");
    f.setPixelSize(11);
    painter.setFont(f);
//    Ttune sT = Ttune::stdTune;
    int nL = 0;
    for (int i=1; i<7; i++) {
        if ( gl->Gtune()[i] != Ttune::stdTune[i])
            nL++;
    }
    int xOffBase = coeff;
    if (nL>3) xOffBase = 0;
    int xOff = xOffBase;
    int yOff = -1;
    int c = 0;
    for (int i=1; i<7; i++) {
        if ( gl->Gtune()[i] != Ttune::stdTune[i]) {
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
             " =" + QString::fromStdString(gl->Gtune()[i].getName(gl->NnameStyleInNoteName,false)));
        painter.drawEllipse(5+xOff,29*coeff+17*yOff,fa,fa);
      }
    }
  }
}

void TscoreWidget::acceptSettings() {
    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    if (gl->Gtune() != Ttune::stdTune)
        setHasScord(true);
    else // resizing is done in setEnableKeySign() method;
        setHasScord(false);
    setEnableKeySign(gl->SkeySignatureEnabled);
    if (!gl->doubleAccidentalsEnabled) clearNote(2);
    noteViews[0]->setPointedColor(gl->SpointerColor);
    noteViews[1]->setColor(gl->enharmNotesColor);
    noteViews[2]->setColor(gl->enharmNotesColor);
    setEnableEnharmNotes(gl->showEnharmNotes);
    if (gl->SkeySignatureEnabled) refreshKeySignNameStyle();
    setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
               Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));
    update();
}

void TscoreWidget::askQuestion(Tnote note, char realStr) {
    setNote(1, note);
    setNoteViewBg(1, gl->EquestionColor);
    m_questMark->show();
    if (realStr) 
      noteViews[1]->setString(realStr);
}

void TscoreWidget::askQuestion(Tnote note, TkeySignature key, char realStr) {
    setKeySignature(key);
    setKeyViewBg(gl->EquestionColor);
    askQuestion(note, realStr);
}

void TscoreWidget::clearScore() {
    clearNote(0);
    setNoteViewBg(0, -1);;
    clearNote(1);
    setNoteViewBg(1, -1);
    noteViews[1]->removeString(); // so far string number to remove occur only on this view
    clearNote(2); // also hide question mark when will be implemented
    if (keySignView) {
        setKeySignature(TkeySignature());
        setKeyViewBg(-1);
        m_questKey->hide();
    }
    changeAccidButtonsState(0); // reset buttons with accidentals
    m_questMark->hide();
}

void TscoreWidget::isExamExecuting(bool isIt) {
    if (isIt) {
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        m_questMark = new QGraphicsSimpleTextItem();
        m_questMark->hide();
        noteViews[2]->scene()->addItem(m_questMark);
        QColor c = gl->EquestionColor;
        c.setAlpha(220);
        m_questMark->setBrush(QBrush(c));
        m_questMark->setText("?");
        resizeQuestMark();
        if (keySignView) {
            m_questKey = new QGraphicsTextItem();
            m_questKey->hide();
            keySignView->scene()->addItem(m_questKey);
        }
    }
    else {
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        delete m_questMark;
        m_questMark = 0;
        delete m_questKey;
        m_questKey = 0;
    }
}

void TscoreWidget::unLockScore() {
    setScoreDisabled(false);
    noteViews[1]->setDisabled(true);
    noteViews[2]->setDisabled(true);
}

void TscoreWidget::setKeyViewBg(QColor C) {
    if (keySignView)
      keySignView->setStyleSheet(gl->getBGcolorText(C));
}

void TscoreWidget::setNoteViewBg(int id, QColor C) {
    noteViews[id]->setStyleSheet(gl->getBGcolorText(C));
}

void TscoreWidget::forceAccidental(Tnote::Eacidentals accid) {
    changeAccidButtonsState(accid);
}

void TscoreWidget::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName) {
    setKeySignature(fakeKey);
    setKeyViewBg(gl->EanswerColor);
    m_questKey->setHtml(QString("<center style=\"color: %1;\"><span style=\"font-family: nootka;\">&nbsp;&nbsp;?&nbsp;&nbsp;</span><br>").arg(gl->EquestionColor.name()) + expectKeyName + "</center>");
    resizeKeyText();
    m_questKey->show();
}

void TscoreWidget::resizeQuestMark() {
    m_questMark->setFont(QFont("nootka", coeff*5));
    m_questMark->setPos(0, coeff*16);    
}

void TscoreWidget::resizeKeyText() {
    int fs = coeff*2;
    do {
        fs--;
        m_questKey->setFont(QFont(font().family(), fs));
    } while (m_questKey->document()->size().width() > keySignView->width());
    m_questKey->setPos(0, coeff*5);
}
