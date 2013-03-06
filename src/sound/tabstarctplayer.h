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


#ifndef TABSTARCTPLAYER_H
#define TABSTARCTPLAYER_H

#include <QObject>

class QTimer;

class TabstarctPlayer : public QObject
{

public:
    TabstarctPlayer(QObject *parent = 0);
    virtual ~TabstarctPlayer();
    
    bool isPlayable() { return playable; }
        /** Immediately stops playing. Emits nothing */
    virtual void stop();
    
signals:
      /** This signal is emited when playing of a note is finished. */
  void noteFinished();
    
protected:
    QTimer *timer;
    bool playable;
    
};

#endif // TABSTARCTPLAYER_H
