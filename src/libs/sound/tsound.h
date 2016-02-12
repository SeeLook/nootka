/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "nootkasoundglobal.h"
#include <QObject>
#include <music/tnote.h>

class Tglobals;
class Tchunk;
class TnoteStruct;
class Tmelody;
class TabstractPlayer;
class TpitchView;
class TaudioIN;

#define   SOUND   Tsound::instance()

/**
 * Tsound is a wrapper of TaudioIN & TaudioOUT classes
 * to manage them. It enables/disables them depends on Tglobals,
 * pauses sniffing when playback is proceeding.
 * Also it has got TpitchView to show volume meter & pitch detection state.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p SOUND
 */
class NOOTKASOUND_EXPORT Tsound : public QObject
{

  Q_OBJECT

public:
  explicit Tsound(QObject *parent = 0);
  virtual ~Tsound();

  TabstractPlayer *player;
  TaudioIN  *sniffer;

  static Tsound* instance() { return m_instance; }

	void play(Tnote& note);
	void playMelody(Tmelody* mel);
  bool isPlayable();
  bool isSniffable() { return (sniffer ? true : false) ; }
  bool melodyIsPlaying() { return m_melodyNoteIndex > -1; }

  TpitchView* pitchView() { return m_pitchView; } /**< First instance existing in main window */

    /** Before Nootka config dialog is created a few things have to be done.
     * stop sniffing, playing, delete midi, which blocks audio devices,
     * delete audioIN, config creates its own to test. */
  void prepareToConf();
  void restoreAfterConf(); /**< Also, when user will discard config, it has to restore its state. */
  void acceptSettings();

  void setPitchView(TpitchView *pView);
  void wait(); /**< Stops sniffing. It is called when en exam is starting. */
  void go(); /**< Starts sniffing again. */

  Tnote& note() { return m_detectedPitch; } /** Returns recently detected note. */
  float pitch(); /**< Returns recently detected pitch of note. */
  void pauseSinffing();
  void unPauseSniffing();
  bool isSnifferPaused();

			/** Prepares sound to exam.
			 * Given notes in params are level range notes and are put to sniffer ambitus. */
  void prepareToExam(Tnote loNote, Tnote hiNote);
  void restoreAfterExam();
  void prepareAnswer(); /**< Sets bg color to question color and enables TpitchView. */
  void restoreAfterAnswer(); /**< Clears bg color and disables TpitchView. */
  void stopPlaying();
	void setDefaultAmbitus(); /**< Instrument scale extended of perfect 4th up and down. */

signals:
	void noteStarted(const Tnote&);
  void noteStartedEntire(const TnoteStruct&);
	void noteFinished(Tchunk*);
  void noteFinishedEntire(const TnoteStruct&);
  void plaingFinished();

private:
  void createPlayer();
  void createSniffer();
  void deletePlayer();
  void deleteSniffer();
  void restoreSniffer(); /**< Brings back sniffer & pitch view state as such as before settings dialog */

  TpitchView 					 *m_pitchView;
  Tnote 			 					m_detectedPitch; // detected note pitch
  bool 				 					m_examMode;
	Tnote				 					m_prevLoNote, m_prevHiNote; // notes form sniffer ambitus stored during an exam
	bool 				 					m_stopSniffOnce, m_userState;;
	int										m_melodyNoteIndex;
	Tmelody							 *m_playedMelody;
  static Tsound        *m_instance;

private slots:
    /** Is performed when note stops playing, then sniffing is unlocked */
  void playingFinishedSlot();
	void playMelodySlot();
	void noteStartedSlot(const TnoteStruct& note);
	void noteFinishedSlot(const TnoteStruct& note);

};

#endif // TSOUND_H
