/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

#ifndef TNOTEPAIR_H
#define TNOTEPAIR_H


#include <QtCore/qobject.h>
#include "music/ttechnical.h"


class Tnote;
class TnoteItem;
class TbeamObject;


/**
 * It wraps @p Tnote and its graphical representation @p TnoteItem.
 * It also handles rhythmical group of a note: @p rhythmGroup() and a beam @p beam()
 */
class TnotePair
{
  Q_GADGET

  friend class TscoreObject;
  friend class TmeasureObject;
  friend class TbeamObject;
  friend class TnoteItem;

public:
  TnotePair(int index = -1, Tnote* n = nullptr, TnoteItem* ob = nullptr);
  ~TnotePair();

  Tnote* note() { return m_note; }
  TnoteItem* item() { return m_noteItem; }

  void setNoteItem(TnoteItem* ob);

      /**
       * Sets note to @p n for both @p note() and then @p item()->setNote()
       */
  void setNote(const Tnote& n);

      /**
       * Sets note pointer to another @p Tnote, doesn't update note of the @p item()
       */
  void setNote(Tnote* n) { m_note = n; }

      /**
       * Bowing, fingering, string number and etc...
       */
  quint32 technical() const { return m_technical.data(); }
  void setTechnical(quint32 tech);

  Ttechnical& techicalData() { return m_technical; }

    /**
     * Number of rhythmical group in the measure, -1 (undefined) by default
     */
  qint8 rhythmGroup() { return m_group; }
  void setRhythmGroup(qint8 g) { m_group = g; }

      /**
       * Number of note in the score
       */
  quint32 index() { return m_index; }
  void setIndex(int i) { m_index = static_cast<quint32>(i); }

      /**
       * Describes what changed in note to be approved into its item
       */
  enum Echanges : quint8 {
    e_noChanges = 0,
    e_stemDirChanged = 1,
    e_beamChanged = 2
  };
  Q_ENUM(Echanges)

  enum Euntie { e_untieNext, e_untiePrev };
  Q_ENUM(Euntie)

  int changes() { return m_changes; }
  void addChange(Echanges ch) { m_changes |= ch;}

      /**
       * Approves @p changes() (if any) to note @p item()
       */
  void approve();

      /**
       * Duration of given list of notes (segments)
       */
  static int pairsDuration(const QList<TnotePair*>& pairs);

protected:
  TbeamObject* beam() { return m_beam; }
  void setBeam(TbeamObject* b);

      /**
       * Disconnects tie. @p untie parameter of @p Euntie enumerator determines:
       * is this note before tie break (@p e_untiePrev) or after (@p e_untieNext)
       * This method doesn't check tie existence it asserts note has tie already.
       * It also takes care about removing of extra tie at the staff beginning
       */
  void disconnectTie(Euntie untie);

      /**
       * Resets this note segment:
       * - sets parent staff of item to null
       * - deletes beam
       * - removes tie, also this extra one at the staff beginning
       */
  void flush();

private:
  Tnote                   *m_note;
  TnoteItem               *m_noteItem;
  qint8                    m_group = -1;
  quint16                  m_index;
  int                      m_changes = 0;
  TbeamObject             *m_beam = nullptr;
  Ttechnical               m_technical;
};

#endif // TNOTEPAIR_H
