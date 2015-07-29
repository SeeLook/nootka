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


#ifndef TQTAUDIOOUT_H
#define TQTAUDIOOUT_H


#include "tabstractplayer.h"

class TaudioParams;
class QMediaPlayer;

/** 
 * 
 */
class NOOTKASOUND_EXPORT TaudioOUT : public TabstractPlayer
{
  Q_OBJECT
   
public:
  explicit TaudioOUT(TaudioParams* _params, QObject* parent = 0);
  virtual ~TaudioOUT();
  
        /** Starts playing given note and then returns true, otherwise gets false. */
  bool play(int noteNr);
  void setAudioOutParams();
  void stop(); /** Immediately stops playing. */

private slots:
	void updateSlot() { setAudioOutParams(); }
	void playingFinishedSlot();
    
private:
  TaudioParams            *m_audioParams;
  QMediaPlayer            *m_player;
};

#endif // TQTAUDIOOUT_H
