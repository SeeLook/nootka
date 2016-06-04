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

#include "tfingerpointer.h"
#include <tpath.h>
#include <tmtr.h>
#include <animations/tmovedanim.h>


TfingerPointer::TfingerPointer(QGraphicsItem* parent) :
  QGraphicsObject(parent),
  m_seqNr(-1)
{
  int s = qRound(Tmtr::fingerPixels() * 1.5);
  m_finger = QPixmap(Tpath::img("fingerpoint")).scaled(s, s, Qt::KeepAspectRatio);
  m_anim = new TmovedAnim(this);
  m_timer = new QTimer(this);
  m_timer->setSingleShot(true);
  connect(m_anim, &TmovedAnim::finished, this, &TfingerPointer::seqFinishedSlot);
  connect(m_timer, &QTimer::timeout, this, &TfingerPointer::timerSlot);
}


TfingerPointer::~TfingerPointer()
{
  stop();
  delete m_anim;
}


void TfingerPointer::start() {
  if (m_moves.isEmpty())
    return;
  m_seqNr = 0;
  moveFinger();
}


void TfingerPointer::stop() {
  m_timer->stop();
}



//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TfingerPointer::moveFinger() {
  m_anim->setDuration(m_moves[m_seqNr].duration);
  m_anim->startMoving(m_moves[m_seqNr].start, m_moves[m_seqNr].stop);
}


void TfingerPointer::seqFinishedSlot() {
  emit moved(m_seqNr);
  m_timer->start(m_moves[m_seqNr].delay);
}


void TfingerPointer::timerSlot() {
  if (m_seqNr < m_moves.count() - 1) {
      m_seqNr++;
      moveFinger();
  } else {
      emit finished();
      m_seqNr = -1;
  }
}




