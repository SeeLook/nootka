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

#include "tstatuslabel.h"
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


TstatusLabel* TstatusLabel::m_instance = nullptr;

TstatusLabel::TstatusLabel(QWidget* parent) :
  TroundedLabel(parent),
  m_text(QString()),
  m_currentBg(-1),
  m_prevBg(-1),
  m_lockStat(false)
{
  if (m_instance) {
    qDebug() << "TstatusLabel instance already exists";
    return;
  }

  m_instance = this;
  setWordWrap(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  setContentsMargins(1, 1, 1, 1); // overwrite 5 px margins of TroundedLabel

  m_messageTimer = new QTimer(this);
  connect(m_messageTimer, &QTimer::timeout, this, &TstatusLabel::restoreMessage);
}


TstatusLabel::~TstatusLabel()
{
  m_instance = nullptr;
}


void TstatusLabel::setMessage(const QString& msg) {
  if (!isVisible())
    return;

  if (!m_lockStat)
      TroundedLabel::setText(center(msg));
  else
      m_prevText = msg;
  m_text = msg;
}


void TstatusLabel::setMessage(const QString& msg, int time) {
  if (!isVisible())
    return;

  m_prevText = m_text;
  TroundedLabel::setText(center(msg));
  m_lockStat = true;
  m_messageTimer->start(time);
}


void TstatusLabel::setBackground(const QColor& bg) {
  if (bg == -1)
      setDefaultBackground();
  else
      setBackroundColor(bg);
  m_currentBg = bg;
}


void TstatusLabel::messageSlot(const QString& msg) {
  if (msg.isEmpty()) { // restore previous message
      setBackground(m_prevBg);
      TroundedLabel::setText(center(m_text));
      m_prevText = m_text;
  } else {
      m_prevBg = m_currentBg;
      setBackground(-1);
      TroundedLabel::setText(center(msg));
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TstatusLabel::restoreMessage() {
  m_messageTimer->stop();
  m_lockStat = false;
  setMessage(m_prevText);
  setBackground(m_prevBg);
  m_prevText.clear();
}



