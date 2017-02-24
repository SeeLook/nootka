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
 * @class TnoteObject is @class QQuickItem derivative representing single note on the score
 */
class NOOTKACORE_EXPORT TnoteObject : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(qreal notePosY READ notePosY WRITE setNotePosY NOTIFY notePosYchanged)

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

signals:
  void notePosYchanged();

protected:
  QString getAccidText();
  QString getHeadText();
  QString getFlagText();

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

private:
  QQuickItem* createAddLine(QQmlComponent& comp);
};

#endif // TNOTEOBJECT_H
