/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tmelodyview.h"
#include "tquestionpoint.h"
#include <score/tscorescene.h>
#include <score/tscorestaff.h>
#include <score/tscorekeysignature.h>
#include <music/tmelody.h>
#include <exam/tqaunit.h>
#include <QScrollBar>
#include <QDebug>



QColor answerColor(quint32 mistake) {
  if (mistake == (quint32)TQAunit::e_correct)
    return TquestionPoint::goodColor();
  else if (!(mistake & TQAunit::e_wrongPos) && !(mistake & TQAunit::e_wrongNote) && !(mistake & TQAunit::e_veryPoor))
    return TquestionPoint::notBadColor();
  else
    return TquestionPoint::wrongColor();
}




TmelodyView::TmelodyView(Tmelody* melody, QWidget* parent) :
  QGraphicsView(parent),
  m_melody(melody),
  m_maxNotes(12)
{
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background-color: transparent;");
  
  TscoreScene *scoreScene = new TscoreScene(this);
  setScene(scoreScene);
  
  for (int s = 0; s <= (m_melody->length() / (m_maxNotes + 1)); ++s) {
    TscoreStaff *staff =  new TscoreStaff(scoreScene, qMin(m_maxNotes, m_melody->length() - s * m_maxNotes));
    staff->onClefChanged(Tclef(m_melody->clef()));
    staff->setStafNumber(s);
    if (m_melody->key().value()) {
      staff->setEnableKeySign(true);
      staff->scoreKey()->setKeySignature(m_melody->key().value());
      staff->scoreKey()->showKeyName(false);
    }
    staff->setDisabled(true);
    for (int n = 0; n < staff->count(); ++n) {
      if (s * m_maxNotes + n < m_melody->length())
        staff->setNote(n, m_melody->note(s * m_maxNotes + n)->p());
      else
        break;
    }
    qreal staffOff = 0.0;
    if (m_melody->clef() == Tclef::e_pianoStaff)
      staffOff = 1.1;
    if (s == 0)
      staff->setPos(staffOff, -staff->hiNotePos());
    else
      staff->setPos(staffOff, m_staves[s - 1]->pos().y() + m_staves[s - 1]->loNotePos() - staff->hiNotePos() + 2.0);
      
    m_staves << staff;
  }
}


void TmelodyView::markMistakes(QList< quint32 > mistakes) {
  for (int i = 0; i < mistakes.size(); ++i) {
    if (i < m_melody->length())
      m_staves[i / m_maxNotes]->noteSegment(i % m_maxNotes)->markNote(answerColor(mistakes[i]));
  }
}


void TmelodyView::clearMistakes() {
  for (int i = 0; i < m_melody->length(); ++i) {
    m_staves[i / m_maxNotes]->noteSegment(i)->markNote(-1);
  }
}


void TmelodyView::resizeEvent(QResizeEvent* event) {
  qreal factor = (((qreal)height() / (m_staves.first()->loNotePos() - m_staves.first()->hiNotePos() + 2.4)) / transform().m11());
  scale(factor, factor);
  qreal sh;
  if (m_staves.size() == 1)
    sh = m_staves.first()->loNotePos() - m_staves.first()->hiNotePos() + 2.0;
  else
    sh = m_staves.last()->pos().y() + m_staves.last()->height() - m_staves.last()->hiNotePos() + 2.0;
  QRectF scRec = m_staves.first()->mapToScene(QRectF(0.0, 0.0, 
                m_staves.first()->width() + (m_melody->clef() == Tclef::e_pianoStaff ? 1.1 : 0.0), sh)).boundingRect();
  scene()->setSceneRect(0.0, 0.0, scRec.width(), scRec.height());
  setFixedWidth(scRec.width() * transform().m11() + horizontalScrollBar()->width() / transform().m11());  
  QGraphicsView::resizeEvent(event);
}










