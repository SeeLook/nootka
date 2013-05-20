/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tscorestaff.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include "tscorenote.h"
#include "tscorekeysignature.h"
#include <QGraphicsView>

#include <QDebug>


TnoteOffset::TnoteOffset(int noteOff, int octaveOff) :
  note(noteOff),
  octave(octaveOff)
{}



TscoreStaff::TscoreStaff(TscoreScene* scene, int notesNr, TscoreStaff::Ekind kindOfStaff) :
  TscoreItem(scene),
  m_kindOfStaff(kindOfStaff),
  m_offset(TnoteOffset(3, 2))
{
  if (m_kindOfStaff == e_normal) {
    m_height = 40;
    m_upperLinePos = 16;
  } else {
    m_height = 20; 
    if (m_kindOfStaff == e_upper)
      m_upperLinePos = 10;
    else if (m_kindOfStaff == e_lower)
      m_upperLinePos = 2;
  }
  setAcceptHoverEvents(true);
// Clef
  Tclef cl = Tclef();
  if (kindOfStaff == e_lower)
    cl = Tclef(Tclef::e_bass_F);
  m_clef = new TscoreClef(scene, this, cl);
  connect(m_clef, SIGNAL(clefChanged()), this, SLOT(onClefChanged()));
  if (kindOfStaff != e_normal)
    m_clef->setReadOnly(true);
// Key signature
  m_keySignature = new TscoreKeySignature(scene, this);
  m_keySignature->setPos(m_clef->boundingRect().width() + 0.5, 0);
  m_keySignature->setClef(m_clef->clef());
  connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
// Notes
  for (int i = 0; i < notesNr; i++) {
      m_notes << new TscoreNote(scene, this, i);
      m_notes[i]->setPos(m_clef->boundingRect().width() + m_keySignature->boundingRect().width() + 0.5 + 
          i * m_notes[i]->boundingRect().width(), 0);
      connect(m_notes[i], SIGNAL(noteWasClicked(int)), this, SLOT(onNoteClicked(int)));
  }
  m_width = m_clef->boundingRect().width() + m_keySignature->boundingRect().width() +
      m_notes.size() * m_notes[0]->boundingRect().width() + 3;  
// Staff lines
  for (int i = 0; i < 5; i++) {
    m_lines[i] = new QGraphicsLineItem();
    registryItem(m_lines[i]);
    m_lines[i]->setPen(QPen(scene->views()[0]->palette().windowText().color(), 0.2));
    m_lines[i]->setLine(1, upperLinePos() + i * 2, boundingRect().width() - 2, upperLinePos() + i * 2);
    m_lines[i]->setZValue(5);
  }
  
  for (int i = 0; i < 7; i++)
    accidInKeyArray[i] = 0;
  
  setStatusTip(tr("This is a staff"));
}


TscoreStaff::~TscoreStaff() {}


QRectF TscoreStaff::boundingRect() const {
  return QRectF(0, 0, m_width, m_height);
}

void TscoreStaff::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(255, 0, 0, 30));
  painter->setPen(Qt::NoPen);
  painter->drawRect(boundingRect());
}



//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreStaff::onClefChanged( ) {  
  int globalNr = notePosRelatedToClef(m_notes[0]->notePos(), m_offset);
  switch(m_clef->clef().type()) {
    case Tclef::e_treble_G:
      m_offset = TnoteOffset(3, 2); break;
    case Tclef::e_treble_G_8down:
      m_offset = TnoteOffset(3, 1); break;
    case Tclef::e_bass_F:
      m_offset = TnoteOffset(5, 0); break;
    case Tclef::e_bass_F_8down:
      m_offset = TnoteOffset(5, -1); break;
    case Tclef::e_alto_C:
      m_offset = TnoteOffset(4, 1); break;
    case Tclef::e_tenor_C:
      m_offset = TnoteOffset(2, 1); break;
  }
  if (m_keySignature)
      m_keySignature->setClef(m_clef->clef());
  int newNr = notePosRelatedToClef(m_notes[0]->notePos(), m_offset);
  for (int i = 0; i < m_notes.size(); i++) {
    m_notes[i]->moveNote(m_notes[i]->notePos() - (globalNr - newNr));
  }
}


void TscoreStaff::onKeyChanged() {
  for (int i = 0; i < m_notes.size(); i++) {
    if (m_notes[i]->notePos())
        m_notes[i]->moveNote(m_notes[i]->notePos());
  }
}


void TscoreStaff::onNoteClicked(int noteIndex) {
  int globalNr = notePosRelatedToClef(m_notes[noteIndex]->notePos(), m_offset);;

  qDebug() << noteIndex << (int)upperLinePos() - globalNr + 1 
  << "NOTE:" << (56 + globalNr) % 7 + 1 
  << "OCTAVE:" << (56 + globalNr) / 7 - 8;
}


