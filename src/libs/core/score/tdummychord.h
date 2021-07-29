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
#include "music/tnote.h"
#include "music/timportscore.h"
#include <QtQuick/qquickitem.h>


class TnoteItem;


/**
 * @class TdummyChord is QML item over @p TnoteItem score note
 * with chord notes from @p TalaChord member.
 * Atop of it there is @p DummyChord.qml which displays those notes.
 * So score import routines create DummyChord instance
 * and add @p setChord() chord with notes.
 * This item when clicked - displays popup with the notes to select.
 * @p selected() is number of selected chord note
 * and when changed - changes @p TalaChord::part->melody() note.
 */
class NOOTKACORE_EXPORT TdummyChord : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString noteHead READ noteHead NOTIFY chordChanged)
  Q_PROPERTY(int chordModel READ chordModel NOTIFY chordChanged)
  Q_PROPERTY(Tmelody* chord READ chord NOTIFY chordChanged)
  Q_PROPERTY(int selected READ selected WRITE setSelected NOTIFY selectedChanged)
  Q_PROPERTY(qreal hiPosY READ hiPosY NOTIFY chordChanged)
  Q_PROPERTY(qreal loPosY READ loPosY NOTIFY chordChanged)
  Q_PROPERTY(bool selectSingle READ selectSingle NOTIFY selectSingleChanged)

public:
  explicit TdummyChord(QQuickItem* parent = nullptr);
  ~TdummyChord();

  int chordModel() const;

  Tmelody* chord() { return m_alaChord->notes(); }
  void setChord(TalaChord* c);

      /**
       * Number of selected note from this chord.
       */
  int selected() const { return m_selected; }
  void setSelected(int s);

  QString noteHead();

  qreal hiPosY() const { return m_hiPosY; }
  qreal loPosY() const { return m_loPosY; }

      /**
       * @p TRUE when single note from the chord is to select,
       * otherwise arpeggiating is done
       */
  bool selectSingle() const;

      /**
       * Handle note item (parent) change.
       * Connect with note parent (@p TscoreObj::destroyed)
       * to reset @p m_parentNote
       */
  void setParentItem(QQuickItem* pi);

  Q_INVOKABLE qreal headPos(int id);
  Q_INVOKABLE QString alterText(int id);
  Q_INVOKABLE QVariant part();

  Q_INVOKABLE void setRhythm();
  Q_INVOKABLE void explodeChord();

signals:
  void chordChanged();
  void selectedChanged();
  void selectSingleChanged();

protected:
  void findHiLoPos();

private:
  TalaChord                 *m_alaChord = nullptr;
  TnoteItem                 *m_parentNote = nullptr;
  int                        m_selected = -1;
  qreal                      m_hiPosY = 50.0, m_loPosY = 0.0;
};

#endif // TDUMMYCHORD_H
