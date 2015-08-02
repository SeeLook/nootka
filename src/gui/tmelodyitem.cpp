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

#include "tmelodyitem.h"
#include <tpath.h>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QAction>
#include <QDebug>


TmelodyItem::TmelodyItem(QAction* playAction, QAction* recordAction) :
  QGraphicsProxyWidget(0),
  m_playAct(playAction), m_recAct(recordAction),
  m_isPlaying(false), m_isRecording(false)
{
  m_button = new QPushButton(QIcon(Tpath::img("melody")), QString(), 0);
  int side = qMin(qApp->screens()[0]->geometry().height(), qApp->screens()[0]->geometry().width()) / 10;
  qDebug() << "pixels per cm:"
           << qApp->screens()[0]->geometry().height() / (qApp->screens()[0]->physicalSize().height() / 10.0) <<
      qApp->screens()[0]->geometry().width() / (qApp->screens()[0]->physicalSize().width() / 10.0);
  m_button->setIconSize(QSize(side, side));
  m_button->setFixedSize(side + 3, side + 3);
  setWidget(m_button);
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  m_holdTime.start();
  QGraphicsProxyWidget::mousePressEvent(event);
}


void TmelodyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (m_holdTime.elapsed() > 50) {
    bool done = false;
    if (m_isPlaying) { // then stop playing
        m_isPlaying = false;
        m_playAct->trigger();
        if (m_isRecording)
          m_button->setIcon(QIcon(Tpath::img("melody-rec")));
        else
          m_button->setIcon(QIcon(Tpath::img("melody")));
        done = true;
    } else if (m_isRecording) { // then stop recording
        m_isRecording = false;
        m_recAct->trigger();
        m_button->setIcon(QIcon(Tpath::img("melody")));
        done = true;
    }
    if (!done) {
      if (m_holdTime.elapsed() < 300) { // short click/tap then start playing
          m_isPlaying = true;
          m_playAct->trigger();
          m_button->setIcon(QIcon(Tpath::img("melody-play")));
      } else { // long click/tap then start recording
          m_isRecording = true;
          m_recAct->trigger();
          m_button->setIcon(QIcon(Tpath::img("melody-rec")));
      }
    }
  }
  QGraphicsProxyWidget::mouseReleaseEvent(event);
}


