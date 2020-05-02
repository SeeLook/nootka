/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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
       * A pointer to note @p index
       */
  Tchunk* note(int index) { return m_notes[index]; }
  Tchunk chunk(int index) const { return *m_notes[index]; }

  Tmeasure& measure(int nr) { return m_measures[nr]; }
  Tmeasure& lastMeasure() { return m_measures.last(); }
  int measuresCount() const { return m_measures.count(); }

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
  bool fromXml(QXmlStreamReader& xml);

  bool saveToMusicXml(const QString& xmlFileName, int transposition = 0);
  bool grabFromMusicXml(const QString& xmlFileName);

      /**
       * Converts given list to melody
       */
  void fromNoteStruct(QList<TnoteStruct>& ns);

private:
  QString              m_title;
  QString              m_composer;
  QList<Tmeasure>      m_measures;
  QList<Tchunk*>       m_notes; /**< List of pointers to ordered notes */
  int                  m_tempo;
  Tmeter::EbeatUnit    m_beat = Tmeter::BeatQuarter;
  TkeySignature        m_key;
  Tmeter              *m_meter = nullptr;
  Tclef::EclefType     m_clef;
};

#endif // TMELODY_H
