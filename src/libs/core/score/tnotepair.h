/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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


class Tnote;
class TnoteObject;
class TbeamObject;


/**
 * It wraps @p Tnote and its graphical representation @p TnoteObject.
 * It also handles rhythmical group of a note: @p rhythmGroup() and a beam @p beam()
 */
class TnotePair
{
  Q_GADGET

  friend class TmeasureObject;
  friend class TbeamObject;
  friend class TnoteObject;

public:
  TnotePair(int index = -1, Tnote* n = nullptr, TnoteObject* ob = nullptr);

  Tnote* note() { return m_note; }
  TnoteObject* item() { return m_noteItem; }

  void setNoteObject(TnoteObject* ob);

    /**
     * Number of rhythmical group in the measure, -1 (undefined) by default
     */
  qint8 rhythmGroup() { return m_group; }
  void setRhythmGroup(qint8 g) { m_group = g; }

      /**
       * Number of note in the score
       */
  quint32 index() { return m_index; }

      /**
       * Describes what changed in note to be approved into its item
       */
  enum Echanges : quint8 {
    e_noChanges = 0,
    e_stemDirChanged = 1,
    e_beamChanged = 2
  };
  Q_ENUM(Echanges)

  int changes() { return m_changes; }
  void addChange(Echanges ch) { m_changes |= ch;}

      /**
       * Approves @p changes() (if any) to note @p item()
       */
  void approve();

protected:
  TbeamObject* beam() { return m_beam; }
  void setBeam(TbeamObject* b);

private:
  Tnote                   *m_note;
  TnoteObject             *m_noteItem;
  qint8                    m_group = -1;
  quint16                  m_index;
  int                      m_changes = 0;
  TbeamObject             *m_beam = nullptr;
};

#endif // TNOTEPAIR_H
