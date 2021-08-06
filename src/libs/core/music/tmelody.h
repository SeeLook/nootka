/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#ifndef TMELODY_H
#define TMELODY_H


#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include "tkeysignature.h"
#include "tclef.h"
#include "tchunk.h"
#include "tmeasure.h"


class Tmeter;
class TnoteStruct;


/**
 * Class describing a musical melody - sequence of notes (Tchunk)
 * Also it is able to save/load a melody into/from MusicXML structure.
 */
class NOOTKACORE_EXPORT Tmelody
{

  friend class TmelodyPart;

public:
  Tmelody(const QString& title = QString(), const TkeySignature& k = TkeySignature());
  Tmelody(const Tmelody& other);
  ~Tmelody();

  QString title() const { return m_title; }
  void setTitle(const QString& t) { m_title = t; }

  QString composer() const { return m_composer; }
  void setComposer(const QString& c) { m_composer = c; }

      /**
       * A length of the melody (notes number)
       */
  int length() const { return m_notes.size(); }

  void addNote(const Tchunk& n);

      /**
       * Deletes all notes from the melody.
       * Also title, composer and resets key, if set to true
       */
  void clear(bool withCredits = true, bool withKey = true);

      /**
       * Copies all notes to given @p chunks list
       */
  void toList(QList<Tchunk>& chunks);

      /**
       * Replaces note @p noteNr with notes in the list.
       * List of notes must have the same duration like replacing note.
       */
  void swapWithNotes(int noteNr, const QList<Tchunk>& notes);

      /**
       * A pointer to note @p index
       */
  Tchunk* note(int index) { return m_notes[index]; }
  Tchunk chunk(int index) const { return *m_notes[index]; }

  Tmeasure& measure(int nr) { return p_measures[nr]; }
  Tmeasure& lastMeasure() { return p_measures.last(); }
  int measuresCount() const { return p_measures.count(); }

  int tempo() const { return m_tempo; }
  void setTempo(int tmp) { m_tempo = tmp; }

      /**
       * Tempo of quarter notes (per minute), independent on beat unit
       */
  int quarterTempo() const { return Tmeter::quarterTempo(m_tempo, m_beat); }

      /**
       * Set both tempo and beat at once
       */
  void setMetronome(int mTempo, Tmeter::EbeatUnit beatUnit) { setTempo(mTempo); setBeat(beatUnit); }

  Tmeter::EbeatUnit beat() const { return m_beat; }
  void setBeat(Tmeter::EbeatUnit bu) { m_beat = bu; }

  TkeySignature key() const { return m_key; }
  void setKey(const TkeySignature& k) { m_key = k; }

  Tclef::EclefType clef() const { return m_clef; }
  void setClef(Tclef::EclefType type) { m_clef = type; }

  Tmeter* meter() const { return m_meter; }
  void setMeter(int m);

      /**
       * @p trans is instrument transposition in semitones,
       * but doesn't changes notes pitch.
       * For music XML structure it is converted to tags:
       * <chromatic> (less than 12)
       * <octave-change> above 12
       * but <diatonic> key is never used so far
       * see: http://usermanuals.musicxml.com/MusicXML/MusicXML.htm#EL-MusicXML-transpose.htm
       */
  void toXml(QXmlStreamWriter& xml, int trans = 0);
  bool fromXml(QXmlStreamReader& xml, bool madeWithNootka = false, int partId = 0);
  bool saveToMusicXml(const QString& xmlFileName, int transposition);
  void writeXmlStream(QXmlStreamWriter &xml, int transposition);
  bool saveToMXL(const QString& xmlFileName, int transposition);

  bool grabFromMusicXml(const QString& xmlFileName);
  bool grabFromMXL(const QString& xmlFileName);

      /**
       * Adds notes of @p otherM (other melody) to this one.
       * Doesn't check for meter/key/clef match
       * and is the last measure full.
       */
  void appendMelody(Tmelody* otherM);

      /**
       * Divides this melody by @p byEveryBar number
       * and stores further parts into @p QList<Tmelody*>.
       */
  void split(int byEveryBar, QList<Tmelody*>& parts);

      /**
       * Transposes all notes in this melody by @p semis semitones:
       * when @p semis is positive - transposes up, when negative - down.
       * @p outScaleToRest determines when notes out of scale is changed to rest
       * or raised/dropped octave down/up (when set to @p FALSE).
       * @p loNote and @p hiNote determines scale (ambitus)
       * in which transposed notes have to fit.
       * But if loNote or hiNote is invalid, checking is skipped.
       * NOTE: When transposition is performed by a key signature
       * set the new key signature before @p transpose() call.
       */
  void transpose(int semis, bool outScaleToRest, const Tnote& loNote, const Tnote& hiNote);

      /**
       * Converts given list to melody
       */
  void fromNoteStruct(QList<TnoteStruct>& ns);

protected:

      /**
       * Common routine to parse musicXML data in @p QXmlStreamReader.
       */
  bool processXMLData(QXmlStreamReader& xml);

      /**
       * Adds @p n note at the beginning of this melody.
       * This method is intended to fill anacrusis measures with rests.
       */
  void prepend(const Tchunk& n);

protected:
  QList<Tmeasure>      p_measures;

private:
  QString              m_title;
  QString              m_composer;
  QList<Tchunk*>       m_notes; /**< List of pointers to ordered notes */
  int                  m_tempo;
  Tmeter::EbeatUnit    m_beat = Tmeter::BeatQuarter;
  TkeySignature        m_key;
  Tmeter              *m_meter = nullptr;
  Tclef::EclefType     m_clef;
};

Q_DECLARE_METATYPE(Tmelody*)

#endif // TMELODY_H
