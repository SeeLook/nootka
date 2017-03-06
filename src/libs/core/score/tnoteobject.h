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

#ifndef TNOTEOBJECT_H
#define TNOTEOBJECT_H


#include "nootkacoreglobal.h"
#include <QtQuick/qquickitem.h>


class TstaffObject;
class TmeasureObject;
class Tnote;


/**
 * @class TnoteObject is @class QQuickItem derivative representing single note on the score.
 * It dynamically crates QML items: note head, alter (accidental) text, stem rectangle and rhythm flag.
 * A tie item form Tie.qml when necessary
 */
class NOOTKACORE_EXPORT TnoteObject : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(qreal notePosY READ notePosY WRITE setNotePosY NOTIFY notePosYchanged)

  friend class TscoreObject;
  friend class TstaffObject;
  friend class TmeasureObject;

public:
  explicit TnoteObject(TstaffObject* staffObj = nullptr);
  ~TnoteObject();

  TstaffObject* staff() const { return m_staff; }
  void setStaff(TstaffObject* staffObj);

  TmeasureObject* measure() { return m_measure; }
  void setMeasure(TmeasureObject* m);

  Tnote* note() { return m_note; }
  void setNote(const Tnote& n);

  qreal notePosY() const { return m_notePosY; }
  qreal setNotePosY(qreal y) { m_notePosY = y; emit notePosYchanged(); }

      /**
       * Note number in the staff
       */
  int index() const { return m_index; }
  void setIndex(int id) { m_index = id; }

  qreal stemHeight() const { return m_stemHeight; }
  void setStemHeight(qreal sh);


  QColor color() { return m_head->property("color").value<QColor>(); }
  void setColor(const QColor& c);

      /**
       * Overrides standard @p setX() method to shift note segment about accidental symbol width (if it is set)
       */
  void setX(qreal xx);

      /**
       * shortcut to X coordinate of right note corner plus gap related to rhythm and staff gap factor
       */
  qreal rightX();

      /**
       * Returns gap factor after this note item depends on current rhythm value
       */
  qreal rhythmFactor();

      /**
       * Prints to std out debug info about this note: [NOTE number] in color
       */
  char debug();

  qreal tieWidth();

signals:
  void notePosYchanged();

protected:
  QString getAccidText();
  QString getHeadText();
  QString getFlagText();

  void keySignatureChanged();

  QQuickItem* tie() { return m_tie; }
  QQuickItem* head() { return m_head; }

      /**
       * @p TnoteObject manages tie itself whenever tie state changes, by calling exactly this method.
       * Tie is simple Text QML item of tie symbol with horizontal scaling that determines tie width,
       * but tie its not aware about next note position as long as next note X coordinate
       * depends on this note width and rhythm factor.
       * So @p setX() method usually called when staff factor is changing, updates tie width.
       * @p tieWidth() returns desired tie width.
       * Also @p TnoteObject objects takes care about breaking tie among staves.
       * It sets tie glyph text to @p m_accidText when note with tie begins a staff
       */
  void checkTie();

private:

  TstaffObject                *m_staff;
  TmeasureObject              *m_measure;
  Tnote                       *m_note;
  int                          m_index;
  qreal                        m_notePosY;
  qreal                        m_x;
  QQuickItem                  *m_head, *m_alter, *m_stem, *m_flag, *m_bg;
  QList<QQuickItem*>           m_upperLines, m_lowerLines;
  qreal                        m_stemHeight;
  QString                      m_accidText;
  QQuickItem                  *m_tie = nullptr;

private:
  QQuickItem* createAddLine(QQmlComponent& comp);
  void updateAlter();
  void updateWidth();
  void updateNoteHead();
  void updateTieScale();
};

#endif // TNOTEOBJECT_H
