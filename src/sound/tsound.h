/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#ifndef TSOUND_H
#define TSOUND_H

#include <QObject>
#include "tnote.h"

class TaudioIN;
class TaudioOUT;

class Tsound : public QObject
{

  Q_OBJECT
  
public:
  explicit Tsound(QObject *parent = 0);
  virtual ~Tsound();
  
  TaudioOUT *player;
  TaudioIN  *sniffer;
  
  void play(Tnote note);
  bool isPlayable() { return m_playable; }
  void acceptSettings();
  
signals:
  void detectedNote(Tnote note);
  
private:
  void createPlayer();
  void createSniffer();
  
  
  bool m_playable, m_sniffable;
  
private slots:
    /** Is performed when note stops plaing, then sniffing is unlocked */
  void playingFinished();
  void noteDetectedSlot(Tnote note);
  
};

#endif // TSOUND_H
