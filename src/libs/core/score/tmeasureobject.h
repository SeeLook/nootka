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

#ifndef TMEASUREOBJECT_H
#define TMEASUREOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


class TscoreObject;
class TstaffObject;
class TnoteObject;
class TnotePair;


/**
 * @class TmeasureObject is an implementation of the measure
 */
class NOOTKACORE_EXPORT TmeasureObject : public QObject
{
  Q_OBJECT

public:

  explicit TmeasureObject(TscoreObject* parent = nullptr);

  int number() const { return m_number; }
  void setNumber(int nr);

  void insertNote(int id, TnotePair* np);

  TscoreObject* score() { return m_score; }

  TstaffObject* staff() { return m_staff; }
  void setStaff(TstaffObject* st);

  TnotePair* first() { return m_notes.first(); }
  TnotePair* last() { return m_notes.last(); }

      /**
       * Staff index of the first measure note
       */
  int firstNoteId() const;

      /**
       * Staff index of the last measure note
       */
  int lastNoteId() const;

private:
  TscoreObject                  *m_score;
  TstaffObject                  *m_staff;
  int                            m_number;
  QList<TnotePair*>              m_notes;

};

#endif // TMEASUREOBJECT_H
