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

#ifndef TSTAFFOBJECT_H
#define TSTAFFOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


class QQuickItem;
class TscoreObject;
class TnoteObject;
class Tnote;


class NOOTKACORE_EXPORT  TstaffObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)
  Q_PROPERTY(qreal upperLine READ upperLine WRITE setUpperLine NOTIFY upperLineChanged)
  Q_PROPERTY(QQuickItem* staffItem READ staffItem WRITE setStaffItem)
  Q_PROPERTY(qreal notesIndent READ notesIndent WRITE setNotesIndent)

public:
  explicit TstaffObject(QObject* parent = nullptr);
  ~TstaffObject();

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* s);

  void addNote(const Tnote& n);

  qreal upperLine() { return m_upperLine; }
  void setUpperLine(qreal upLine);

  QQuickItem* staffItem() { return m_staffItem; }
  void setStaffItem(QQuickItem* si);

  qreal notesIndent() { return m_notesIndent; }
  void setNotesIndent(qreal ni) { m_notesIndent = ni; }

signals:
  void noteAdded(TnoteObject* note);
  void upperLineChanged();

private:
  TscoreObject                  *m_score;
  qreal                          m_upperLine;
  QList<TnoteObject*>            m_notes;
  QQuickItem                    *m_staffItem;
  qreal                          m_notesIndent;
};

#endif // TSTAFFOBJECT_H
