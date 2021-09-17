/***************************************************************************
 *   Copyright (C) 2015-2021 by Tomasz Bojczuk                             *
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


#ifndef TCOMMONLISTENER_H
#define TCOMMONLISTENER_H


#include "nootkasoundglobal.h"
#include "tabstractsniffer.h"


class TpitchFinder;
class TaudioParams;


/**
 * Common class for managing a pitch detection process.

 * It wraps TpitchFinder values - adjust middle a offset and rejects notes out or range (ambitus)
 *
 * It can be paused - pitch detection is performed but signals are not sending
 * or it can be stopped - incoming audio data is ignored
 * It has pure virtual methods @p startListening(), @p stopListening()
 * those have to be implemented by subclass.
 */
class NOOTKASOUND_EXPORT TcommonListener : public TabstractSniffer
{

  Q_OBJECT

public:
  explicit TcommonListener(TaudioParams* params, QObject* parent = nullptr);
  ~TcommonListener();


      /**
       * Stops emitting signals about pitch detection, but detection is still performed.
       * It also resets last chunk pitch to ignore detection
       * It helps to sniff whole sound/note from begin to its end.
       */
  void pause() {
      m_LastChunkPitch = 0.0;
      if (detectingState() == e_detecting)
        setState(e_paused);
  }

      /**
       * Instance of @p TpitchFinder
       */
  TpitchFinder* finder() { return m_pitchFinder; }

      /**
       * Current volume of detecting sound or 0 if silence
       */
  float volume() { return m_volume; }

      /**
       * Volume of raw PCM signal
       */
  qreal pcmVolume();

    /**
     * Sets device parameters stores in structure SaudioInParams.
     * SaudioInParams::deviceName is ignored. It have to be set separately.
     */
  virtual void setAudioInParams();

      /**
       * Sets minimal volume needed that note will be detected. Overrides global setting.
       */
  void setMinimalVolume(qreal minVol);
  qreal minimalVolume();

  void setAmbitus(const Tnote& lowestNote, const Tnote& highestNote);

      /**
       * Duration time of a single chunk for current sample rate and frames per chunk.
       */
  qreal chunkTime() const;

      /**
       * Pitch of last detected note in double precision.
       */
  qreal lastNotePitch() { return p_lastNote.pitchF; }

      /**
       * Pitch, frequency and duration of the last detected note.
       */
  TnoteStruct& lastNote() { return p_lastNote; }

  float lastChunkPitch() const { return m_LastChunkPitch; }

      /**
       * Returns @p TRUE when @p pitch is in ambitus
       */
  bool inRange(qreal pitch) const { return (pitch >= m_loPitch && pitch <= m_hiPitch); }

      /**
       * @p TRUE when note started but not finished.
       */
  bool noteWasStarted() const { return p_noteWasStarted; }

      /**
       * Sets pitch detection method. Currently three are available:
       * 0 - MPM (Philip McLeod Method) - default
       * 1 - auto-correlation
       * 2 - MPM modified cepstrum.
       * Currently set value is available through global.
       */
  void setDetectionMethod(int method);

      /**
       * Returns intonation accuracy sets in global audio settings.
       */
  quint8 intonationAccuracy();

      /**
       * Sets global value of intonation accuracy. It doesn't refresh intonation view.
       */
  void setIntonationAccuracy(qint8 intAcc);

  int detectionRange() { return m_currentRange; } /**< Integer value of @p TpitchFinder::Erange */
#if !defined (Q_OS_ANDROID)
  void setDumpFileName(const QString& fName);
#endif


signals:

      /**
       * Emitted when raw PCM volume is too high or too low for a few detected notes
       */
  void lowPCMvolume();
  void hiPCMvolume();

  void volumeChanged();

protected:
      /**
       * Sets volume to 0
       */
  void resetVolume() { m_volume = 0.0; }
  void resetChunkPitch() { m_LastChunkPitch = 0.0; }


protected slots:
  void pitchInChunkSlot(float pitch);
  void volumeSlot(float vol) { m_volume = vol; emit volumeChanged(); }
  void updateSlot() { setAudioInParams(); }
  void noteStartedSlot(qreal pitch, qreal freq, qreal duration);
  void noteFinishedSlot(TnoteStruct* lastNote);


private:
  TpitchFinder     *m_pitchFinder;
  float             m_volume;
  float             m_LastChunkPitch; /**< Pitch from recent processed chunk or 0.0 if silence */
  qreal             m_loPitch, m_hiPitch;
  int               m_currentRange; /**< Current range of detected note - see @p TaudioParams */
  int               m_loPCMnumber = 0, m_hiPCMnumber = 0; /**< Counts number of PCM volumes out of range, to emit warning note  */
};

#endif // TCOMMONLISTENER_H
