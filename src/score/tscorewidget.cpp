/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tscorewidget.h"
#include "tnoteview.h"
#include "tkeysignatureview.h"
#include "tglobals.h"
#include "tkeysignature.h"
#include "tscordatureview.h"
#include "tgraphicstexttip.h"
#include <QtGui>


extern Tglobals *gl;

TscoreWidget::TscoreWidget(unsigned char _notesCount, QWidget *parent) :
    TscoreWidgetSimple(_notesCount, parent),
    m_scordature(0)
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

    setScordature();
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
    setMinimumWidth(qRound((qreal)height() / 1.15) + 56);
    TscoreWidgetSimple::resizeEvent(event);    
    if (m_questMark) {
        resizeQuestMark();
    }
    if (m_questKey && m_questKey->isVisible())
        resizeKeyText();
    if (m_scordature)
#if defined(Q_OS_MAC)
        m_scordature->setGeometry(0, 29 * coeff, 13 * coeff, 6 * coeff);
#else
        m_scordature->setGeometry(0, 29 * coeff, 12 * coeff, 6 * coeff);
#endif
}

void TscoreWidget::setScordature() {
    if (gl->Gtune() != Ttune::stdTune) {
        if (m_scordature)
          delete m_scordature;
        setHasScord(true);
        m_scordature = new TscordatureView(this);
#if defined(Q_OS_MAC)
        m_scordature->setGeometry(0, 29 * coeff, 13 * coeff, 6 * coeff);
#else
        m_scordature->setGeometry(0, 29 * coeff, 12 * coeff, 6 * coeff);
#endif

        m_scordature->show();
        m_scordature->setTune(gl->Gtune(), gl->NnameStyleInNoteName);
        m_scordature->update();
    } else { // resizing is done in setEnableKeySign() method;
        setHasScord(false);
        if (m_scordature) {
          delete m_scordature;
          m_scordature = 0;
        }
    }
}


void TscoreWidget::acceptSettings() {
    setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    setScordature();
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
//     setNoteViewBg(1, gl->EquestionColor);
    setBGcolor(gl->mergeColors(gl->EquestionColor, palette().window().color()));
    m_questMark->show();
    if (realStr) 
      noteViews[1]->setString(realStr);
}

void TscoreWidget::askQuestion(Tnote note, TkeySignature key, char realStr) {
    setKeySignature(key);
//     setKeyViewBg(gl->EquestionColor);
    askQuestion(note, realStr);
}



void TscoreWidget::clearScore() {
    clearNote(0);
    clearNote(1);
    noteViews[1]->removeString(); // so far string number to remove occur only on this view
    clearNote(2); // also hide question mark when will be implemented
    if (keySignView) {
        setKeySignature(TkeySignature());
        keySignView->setStyleSheet("background: transparent");
        if (m_questKey) {
          delete m_questKey;
          m_questKey = 0;
        }
    }
    changeAccidButtonsState(0); // reset buttons with accidentals
    m_questMark->hide();
    noteViews[0]->setStyleSheet("background: transparent");
    setBGcolor(palette().base().color());
}

void TscoreWidget::markAnswered(QColor blurColor) {
    noteViews[0]->markNote(blurColor);
}

void TscoreWidget::markQuestion(QColor blurColor) {
    noteViews[1]->markNote(blurColor);
}


void TscoreWidget::isExamExecuting(bool isIt) {
    if (isIt) {
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
        m_questMark = new QGraphicsSimpleTextItem();
        m_questMark->hide();
        noteViews[2]->scene()->addItem(m_questMark);
        QColor c = gl->EquestionColor;
        c.setAlpha(220);
        noteViews[1]->setColor(c);
        m_questMark->setBrush(QBrush(c));
        m_questMark->setText("?");
        resizeQuestMark();
    }
    else {
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
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
    if (m_questMark) {
      setBGcolor(gl->mergeColors(gl->EanswerColor, palette().window().color()));
//       noteViews[0]->setStyleSheet(QString("background: transparent; border: 1px solid %1; border-radius: 10px;").arg(gl->EanswerColor.name()));
      noteViews[0]->setStyleSheet(gl->getBGcolorText(gl->EanswerColor) + "border-radius: 10px;");
    }
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
    m_questKey = new QGraphicsTextItem();
    keySignView->scene()->addItem(m_questKey);
    m_questKey->setHtml(QString("<span style=\"color: %1;\"><span style=\"font-family: nootka;\">?</span><br>").arg(gl->EquestionColor.name()) + expectKeyName + "</span>");
    TgraphicsTextTip::alignCenter(m_questKey);
    resizeKeyText();
//     keySignView->setStyleSheet(QString("background: transparent; border: 1px solid %1; border-radius: 10px;").arg(gl->EanswerColor.name()));
    keySignView->setStyleSheet(gl->getBGcolorText(gl->EanswerColor) + "border-radius: 10px;");
}

void TscoreWidget::resizeQuestMark() {
#if defined(Q_OS_MACX)
    m_questMark->setFont(QFont("nootka", coeff * 7));
#else
    m_questMark->setFont(QFont("nootka", coeff * 5));
#endif
    m_questMark->setPos(0, coeff * 16);
}

void TscoreWidget::resizeKeyText() {
  if (!m_questKey)
    return;
  qreal sc = keySignView->width() / m_questKey->boundingRect().width();
  m_questKey->setScale(sc);
  m_questKey->setPos(0, coeff * 5);
}

void TscoreWidget::expertNoteChanged() {
    emit noteClicked();
}

