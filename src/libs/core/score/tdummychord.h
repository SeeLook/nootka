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

#ifndef TDUMMYCHORD_H
#define TDUMMYCHORD_H


#include <nootkacoreglobal.h>
#include <music/tnote.h>
#include <QtQuick/qquickitem.h>


class TnoteItem;
class Tmelody;

/**
 * @todo write docs
 */
class NOOTKACORE_EXPORT TdummyChord : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString noteHead READ noteHead NOTIFY chordChanged)
  Q_PROPERTY(int chordModel READ chordModel NOTIFY chordChanged)
  Q_PROPERTY(Tmelody* chord READ chord NOTIFY chordChanged)

public:
  explicit TdummyChord(QQuickItem* parent = nullptr);
  ~TdummyChord();

  int chordModel() const;

  Tmelody* chord() { return m_chord; }
  void setChord(Tmelody* c);

  QString noteHead();

  Q_INVOKABLE qreal headPos(int id);
  Q_INVOKABLE QString alterText(int id);

signals:
  void chordChanged();

private:
  Tmelody                   *m_chord = nullptr;
  TnoteItem                 *m_parentNote = nullptr;
};

#endif // TDUMMYCHORD_H
