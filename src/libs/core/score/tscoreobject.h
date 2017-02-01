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

#ifndef TSCOREOBJECT_H
#define TSCOREOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


/**
 * Describes offset of a note.
 */
class TclefOffset
{
public:
  TclefOffset(qint8 noteOff = 0, qint8 octaveOff = 0) : note(noteOff), octave(octaveOff) {}

  qint8 note;
  qint8 octave;
  int total() { return octave * 7 + note; }
};



class Tnote;
class TstaffObject;
class TnoteObject;
class Tmeter;



class NOOTKACORE_EXPORT  TscoreObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(QObject* parent READ parent WRITE setParent)

  friend class TstaffObject;
  friend class TnoteObject;

public:
  explicit TscoreObject(QObject* parent = nullptr);
  ~TscoreObject();

  void setParent(QObject* p);

  Q_INVOKABLE void addNote(const Tnote& n);
  Q_INVOKABLE void setNote(int staffNr, int noteNr, const Tnote& n);

  bool keySignatureEnabled() const { return m_keySignEnabled; }
  void setKeySignatureEnabled(bool enKey);

protected:
  void addStaff(TstaffObject* st);

  TclefOffset clefOffset() const { return m_clefOffset; }

private:
  Tmeter                           *m_meter;
  bool                              m_keySignEnabled;
  TclefOffset                       m_clefOffset;
  QList<TstaffObject*>              m_staves;

};

#endif // TSCOREOBJECT_H
