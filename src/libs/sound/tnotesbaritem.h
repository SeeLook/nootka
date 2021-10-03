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


#include "nootkasoundglobal.h"
#include <music/tnote.h>

#include <QtQuick/qquickpainteditem.h>


class TnoteStruct;


/**
 * @class TnotesBarItem is C++ logic of QML @p NotesBar item,
 * which visualizes distance between @p expectedNote() and @p detectedNote()
 * and shows names of those notes.
 * Also it draws lines (ticks) representing semitones of difference
 */
class NOOTKASOUND_EXPORT TnotesBarItem : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(Tnote expectedNote READ expectedNote WRITE setExpectedNote NOTIFY expectedNoteChanged)
  Q_PROPERTY(Tnote detectedNote READ detectedNote WRITE setDetectedNote NOTIFY detectedNoteChanged)
  Q_PROPERTY(QString expectedName READ expectedName NOTIFY expectedNoteChanged)
  Q_PROPERTY(qreal expectedX READ expectedX NOTIFY expectedNoteChanged)
  Q_PROPERTY(QString detectedName READ detectedName NOTIFY detectedNoteChanged)
  Q_PROPERTY(qreal detectedX READ detectedX NOTIFY detectedNoteChanged)
  Q_PROPERTY(qreal notesDiff READ notesDiff NOTIFY detectedNoteChanged)
  Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)

public:
  TnotesBarItem(QQuickItem* parent = nullptr);


  Tnote expectedNote() const { return m_expectedNote; }
  void setExpectedNote(const Tnote &expectedNote);
  QString expectedName() const { return m_expectedNote.isValid() ? m_expectedNote.styledName() : QStringLiteral("--"); }

  Tnote detectedNote() const { return m_detectedNote; }
  void setDetectedNote(const Tnote &detectedNote);
  QString detectedName() const { return m_detectedNote.isValid() ? m_detectedNote.styledName() : QString(); }

  qreal expectedX() const;
  qreal detectedX() const;

  qreal notesDiff() const;

  bool isPlaying() const { return m_isPlaying; }

  Tnote lowestNote() const { return m_lowestNote; }
  Tnote highestNote() const { return m_highestNote; }

  Q_INVOKABLE void setAmbitus(const Tnote& lo, const Tnote& hi);

  void paint(QPainter *painter) override;

  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

signals:
  void detectedNoteChanged();
  void expectedNoteChanged();
  void isPlayingChanged();


protected:
  void noteStartedSlot(const Tnote& n);
  void noteFinishedSlot(const TnoteStruct& n);


private:
  Tnote               m_lowestNote;
  Tnote               m_highestNote;
  Tnote               m_detectedNote;
  Tnote               m_expectedNote;
  int                 m_notesSpan = 1;
  bool                m_isPlaying = false;
};

#endif // TNOTESBARITEM_H
