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

#ifndef TMELODYITEM_H
#define TMELODYITEM_H

#include <QGraphicsProxyWidget>
#include <QTime>


class QPushButton;

/**
 * This is QPushButton into proxy to switch melody to play
 * or to record mode
 */
class TmelodyItem : public QGraphicsProxyWidget
{
  Q_OBJECT

public:
  TmelodyItem(QAction* playAction, QAction* recordAction);

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
  QPushButton           *m_button;
  QTime                  m_holdTime;
  QAction               *m_playAct, *m_recAct;
  bool                   m_isPlaying, m_isRecording;

};

#endif // TMELODYITEM_H
