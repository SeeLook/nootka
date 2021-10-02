/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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


#ifndef TNOTESBARITEM_H
#define TNOTESBARITEM_H


#include "nootkacoreglobal.h"
#include "music/tnote.h"

#include <QtQuick/qquickitem.h>

/**
 * @class TnotesBarItem is C++ logic of QML @p NotesBar item,
 * which visualizes distance between @p expectedNote() and @p detectedNote()
 * and shows names of those notes.
 */
class TnotesBarItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(Tnote expectedNote READ expectedNote WRITE setExpectedNote NOTIFY expectedNoteChanged)
  Q_PROPERTY(Tnote detectedNote READ detectedNote WRITE setDetectedNote NOTIFY detectedNoteChanged)
  Q_PROPERTY(int lowestNote READ lowestNote WRITE setLowestNote NOTIFY lowestNoteChanged)
  Q_PROPERTY(int highestNote READ highestNote WRITE setHighestNote NOTIFY highestNoteChanged)
  Q_PROPERTY(QString expectedName READ expectedName NOTIFY expectedNoteChanged)
  Q_PROPERTY(QString detectedName READ detectedName NOTIFY detectedNoteChanged)

public:
  TnotesBarItem(QQuickItem* parent = nullptr);


  Tnote expectedNote() const { return m_expectedNote; }
  void setExpectedNote(const Tnote &expectedNote);
  QString expectedName() const { return m_expectedNote.isValid() ? m_expectedNote.styledName() : QStringLiteral("--"); }

  Tnote detectedNote() const { return m_detectedNote; }
  void setDetectedNote(const Tnote &detectedNote);
  QString detectedName() const { return m_detectedNote.isValid() ? m_detectedNote.styledName() : QString(); }

  int lowestNote() const { return m_lowestNote; }
  void setLowestNote(int lowestNote);

  void setHighestNote(int highestNote);
  int highestNote() const { return m_highestNote; }

signals:
  void lowestNoteChanged();
  void highestNoteChanged();
  void detectedNoteChanged();
  void expectedNoteChanged();

private:
  int                 m_lowestNote;
  int                 m_highestNote;
  Tnote               m_detectedNote;
  Tnote               m_expectedNote;
};

#endif // TNOTESBARITEM_H
