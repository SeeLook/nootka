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

class TpitchView;
class TaudioIN;
class TaudioOUT;

/** Tsound is a wrapper of TaudioIN & TaudioOUT classes
 * to manage them. It enables/disables them depends on Tglobals,
 * pauses sniffing when playback is proceding.
 * Also it has got TpitchView to show volume meter & pitch detection state.
 */
class Tsound : public QObject
{

  Q_OBJECT
  
public:
  explicit Tsound(QObject *parent = 0);
  virtual ~Tsound();
  
  TaudioOUT *player;
  TaudioIN  *sniffer;
  
  void play(Tnote note);
  bool isPlayable() { return (player ? true : false) ; }
  bool isSniffable() { return (sniffer ? true : false) ; }
    /** Before Nootka config dialog is created a few things have to be done.
     * stop sniffing, playing
     * delete midi, whitch bloks audio devices,
     * delete audioIN, config creates its own to test. */
  void prepareToConf();
    /** Also, when user will discard config, it has to restore its state. */
  void restoreAfterConf();
  void acceptSettings();
  void setPitchView(TpitchView *pView);
    /** Stops sniffing. It is called when en exam is starting. */
  void wait();
    /** Starts sniffing again. */
  void go();
    /** Returns recently detected note. */
  Tnote note() { return m_detNote; }
  void prepareToExam();
  void restoreAfterExam();
  void prepareAnswer();
  void restoreAfterAnswer();
  
signals:
  void detectedNote(Tnote note);
  
private:
  void createPlayer();
  void createSniffer();
  void deletePlayer();
  void deleteSniffer();
  
  TpitchView *m_pitchView;
  Tnote m_detNote; // detected note
  
private slots:
    /** Is performed when note stops playing, then sniffing is unlocked */
  void playingFinished();
  void noteDetectedSlot(Tnote note);
   /** under MacOs first call of sniffer->startListening() has to be delayed */
  void macSlot();



};

#endif // TSOUND_H
