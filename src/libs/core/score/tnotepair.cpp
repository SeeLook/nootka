/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#include "tnotepair.h"
#include "music/tnote.h"
#include "tnoteitem.h"
#include "tstaffitem.h"
#include "tbeamobject.h"


TnotePair::TnotePair(int index, Tnote* n, TnoteItem* ob) :
  m_note(n),
  m_noteItem(ob),
  m_index(static_cast<quint16>(index))
{

}


TnotePair::~TnotePair()
{
  delete m_noteItem;
}


// void TnotePair::setIndex(int i) {
//   m_index = static_cast<quint32>(i);
//   if (m_noteItem)
//     m_noteItem->updateDebug();
// }
// 
// 
// void TnotePair::setRhythmGroup(qint8 g) {
//   m_group = g;
//   if (m_noteItem)
//     m_noteItem->updateDebug();
// }


void TnotePair::setNoteItem(TnoteItem* ob) {
  m_noteItem = ob;
}


void TnotePair::setNote(const Tnote& n) {
  *m_note = n;
  m_noteItem->setNote(n);
}


void TnotePair::setTechnical(quint32 tech) {
  if (tech != m_technical.data()) {
    Ttechnical newTechn(tech);
    if (newTechn.fingerPos().str() != m_technical.fingerPos().str())
      m_noteItem->setStringNumber(newTechn.fingerPos().str());
    if (newTechn.bowing() != m_technical.bowing())
      m_noteItem->setBowing(static_cast<TnoteItem::EbowDirection>(newTechn.bowing()));
    m_technical.setData(tech);
  }
}


void TnotePair::approve() {
  if (m_changes) {
    if (m_changes & e_beamChanged || m_changes & e_stemDirChanged)
      m_noteItem->setNote(*m_note);
    m_changes = 0;
  }
}


int TnotePair::pairsDuration(const QList<TnotePair*>& pairs) {
  int dur = 0;
  for (TnotePair* np : qAsConst(pairs))
    dur += np->note()->duration();
  return dur;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TnotePair::setBeam(TbeamObject* b) {
  m_beam = b;
}


void TnotePair::disconnectTie(Euntie untie) {
  Trhythm::Etie t;
  if (untie == e_untieNext)
    t = m_note->rtm.tie() == Trhythm::e_tieCont ? Trhythm::e_tieStart : Trhythm::e_noTie;
  else // e_untiePrev
    t = m_note->rtm.tie() == Trhythm::e_tieCont ? Trhythm::e_tieEnd : Trhythm::e_noTie;
  m_note->rtm.setTie(t);
  m_noteItem->note()->rtm.setTie(t);
  m_noteItem->checkTie();
  if (this == m_noteItem->staff()->firstNote() && (t == Trhythm::e_noTie || t == Trhythm::e_tieStart))
    m_noteItem->staff()->deleteExtraTie();
}


void TnotePair::flush() {
  m_noteItem->markNoteHead(Qt::transparent);
  m_noteItem->setNoteNameVisible(false);
  if (m_beam && this == m_beam->last())
    m_beam->deleteBeam(); // it will reset this beam to nullptr
  if (this == m_noteItem->staff()->firstNote())
    m_noteItem->staff()->deleteExtraTie();
  if (m_noteItem->tie()) {
    m_noteItem->note()->rtm.setTie(Trhythm::e_noTie);
    m_noteItem->checkTie();
  }
  m_noteItem->setStaff(nullptr);
  m_noteItem->setBowing(TnoteItem::BowUndefined);
  m_noteItem->setStringNumber(0);
  m_technical.reset();
}
