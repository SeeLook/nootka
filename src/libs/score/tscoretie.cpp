/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "tscoretie.h"
#include "tscorenote.h"
#include <music/tnote.h>
#include <tnoofont.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtGui/qbrush.h>

#include <QtCore/qdebug.h>

/**
 * @p TscoreTie class supports all kinds of tie combinations.
 * As long as graphically tie bow is an independent single element between two notes
 * and it is not aware about others
 * logically, through @p Tnote::rtm.tie many notes can be connected.
 *
 * @p TrhythmPane calls @p TscoreTie::check when user clicks tie item on it
 *
 * TODO: - graphically a bow doesn't look like score engraving tradition used to do.
 *       - extra tie at the staff beginning collides with key signature
 */

TscoreTie::TscoreTie(TscoreNote* sn1, TscoreNote* sn2) :
  m_firstNote(sn1),
  m_scondNote(sn2)
{
  if (firstNote()->note()->rtm.tie()) // continue tie if first note already belongs to someone
    firstNote()->note()->rtm.setTie(Trhythm::e_tieCont);
  else // or start a new one
    firstNote()->note()->rtm.setTie(Trhythm::e_tieStart);

  if (secondNote()->tie()) // second note has a tie so set it to continue
    secondNote()->note()->rtm.setTie(Trhythm::e_tieCont);
  else // or end the tie on second note
    secondNote()->note()->rtm.setTie(Trhythm::e_tieEnd);

  m_tieItem = newTie(firstNote());
  updateLength();
  checkStaves();
}


TscoreTie::~TscoreTie()
{
  qDebug() << "[TIE] deleting" << firstNote()->index() << firstNote()->note()->toText() << "with" << secondNote()->index();
  Trhythm& firstRtm = firstNote()->note()->rtm;
  if (firstRtm.tie() == Trhythm::e_tieCont)
    firstRtm.setTie(Trhythm::e_tieEnd);
  else
    firstRtm.setTie(Trhythm::e_noTie);
  firstNote()->setTie(nullptr);

  Trhythm& secondRtm = secondNote()->note()->rtm;
  if (secondRtm.tie() == Trhythm::e_tieCont) // when second note has a tie - set it to tie start
    secondRtm.setTie(Trhythm::e_tieStart);
  else
    secondRtm.setTie(Trhythm::e_noTie);

  delete m_tieItem;
  if (m_extraTieItem)
    delete m_extraTieItem;
}


/*static*/
void TscoreTie::check(TscoreNote* sn) {
  if (sn->tie())
      delete sn->tie();
  else {
      auto next = sn->nextNote();
      if (!sn->note()->isRest() && next && !next->note()->isRest() && sn->note()->compareNotes(*next->note()))
          sn->setTie(new TscoreTie(sn, next));
  }
}


void TscoreTie::checkStaves() {
  if ((m_extraTieItem == nullptr) != (firstNote()->staff() == secondNote()->staff())) {
      if (m_extraTieItem) {
          delete m_extraTieItem;
      } else {
          m_extraTieItem = newTie(secondNote());
          QTransform t;
          t.scale(1.0, secondNote()->note()->rtm.stemDown() ? -1.0 : 1.0);
          m_extraTieItem->setTransform(t);
          m_extraTieItem->setPos(-m_extraTieItem->boundingRect().width(),
                                 secondNote()->notePos() + (secondNote()->note()->rtm.stemDown() ? 3.7 : - 1.8));
      }
      updateLength();
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreTie::updateLength() {
  QTransform t;
  if (firstNote()->staff() == secondNote()->staff())
    t.scale((secondNote()->x() - firstNote()->x() - firstNote()->width()) / m_tieItem->boundingRect().width(),
            firstNote()->note()->rtm.stemDown() ? -1.0 : 1.0);
  else
    t.scale(1.0, firstNote()->note()->rtm.stemDown() ? -1.0 : 1.0);
  m_tieItem->setTransform(t);
  m_tieItem->setPos(firstNote()->width(), firstNote()->notePos() + (firstNote()->note()->rtm.stemDown() ? 3.7 : - 1.8));
}


QGraphicsSimpleTextItem * TscoreTie::newTie(TscoreNote* parentNote) {
  auto tie = new QGraphicsSimpleTextItem(QString(QChar(0xe18c)), parentNote); // tie symbol
  tie->setFont(TnooFont(5));
  tie->setBrush(QBrush(parentNote->color()));
  return tie;
}







