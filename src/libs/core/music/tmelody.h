/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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
#include "tmeasure.h"
#include "tclef.h"

class Tmeter;
class TnoteStruct;

/**
 * Class describing a musical melody - sequence of notes (Tchunk)
 * Also it is able to save/load a melody into/from MusicXML structure
 * Default tempo of a melody is 120 bpm.
 */
class NOOTKACORE_EXPORT Tmelody
{

public:
  Tmelody(const QString& title = QString(), const TkeySignature& k = TkeySignature());

  QString& title() {return m_title; }
  void setTitle(const QString& t) { m_title = t; }

  int length() { return m_notes.size(); } /** A length of the melody (notes number) */

  void addNote(const Tchunk& n);
  Tchunk* note(int index) { return m_notes[index]; } /** A pointer to note @p index */

  Tmeasure& measure(int nr) { return m_measures[nr]; }
  Tmeasure& lastMeasure() { return m_measures.last(); }

  int tempo() { return m_tempo; }
  void setTempo(int tmp) { m_tempo = tmp; }

  TkeySignature key() { return m_key; }
  void setKey(const TkeySignature& k) { m_key = k; }

  Tclef::EclefType clef() { return m_clef; }
  void setClef(Tclef::EclefType type) { m_clef = type; }

  void toXml(QXmlStreamWriter& xml);
  bool fromXml(QXmlStreamReader& xml);

  bool saveToMusicXml(const QString& xmlFileName);
  bool grabFromMusicXml(const QString& xmlFileName);

  void fromNoteStruct(QList<TnoteStruct>& ns); /**< Converts given list to melody */

private:
  QString              m_title;
  QList<Tmeasure>      m_measures;
  QList<Tchunk*>       m_notes; /**< List of pointers to ordered notes */
  int                  m_tempo;
  TkeySignature        m_key;
  Tmeter              *m_meter;
  Tclef::EclefType     m_clef;
};

#endif // TMELODY_H
