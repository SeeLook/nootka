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


#ifndef TABSTRACTPLAYER_H
#define TABSTRACTPLAYER_H

#include <QObject>

class QTimer;


/** Base abstract class for sound output (plaing scale). */
class TabstractPlayer : public QObject
{

  Q_OBJECT
  
public:
    TabstractPlayer(QObject *parent = 0);
    virtual ~TabstractPlayer();
    
    bool isPlayable() { return playable; }
    
        /** Starts plaing given note and then returns true, otherwise gets false. */
    virtual bool play(int noteNr);
        /** Immediately stops playing. Emits nothing */
    virtual void stop();
        /**Does nothing in audio player subclass. */
    virtual void deleteMidi(); 
    
    virtual void setMidiParams();
    
signals:
      /** This signal is emited when playing of a note is finished. */
  void noteFinished();
    
  
protected:
    bool playable;
        /** Determines whether noteFinished() signal is emited in offTimer timeOut() slot.
         * Slot is also called by stop() method and then signal can't be emited. */
    bool doEmit;
    QTimer *offTimer;
    
};

#endif // TABSTRACTPLAYER_H
