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

#ifndef TBANDONEONBG_H
#define TBANDONEONBG_H


#include <QtCore/qobject.h>
#include "music/tnote.h"


class TbandoneonBg : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(bool closing READ closing WRITE setClosing NOTIFY closingChanged)
  Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)

public:
  TbandoneonBg(QObject* parent = nullptr);

  Q_INVOKABLE qreal leftXat(int b);
  Q_INVOKABLE qreal leftYat(int b);
  Q_INVOKABLE int leftOpenAt(int b);
  Q_INVOKABLE int leftCloseAt(int b);

  Q_INVOKABLE qreal rightXat(int b);
  Q_INVOKABLE qreal rightYat(int b);
  Q_INVOKABLE int rightOpenAt(int b);
  Q_INVOKABLE int rightCloseAt(int b);

  int currentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int i);

  bool closing() const { return m_isClosing; }
  void setClosing(bool c);

  Tnote note() const { return m_note; }
  void setNote(const Tnote& n);

signals:
  void currentIndexChanged();
  void closingChanged();
  void noteChanged();

private:
  void getNote();

private:
  int               m_currentIndex;
  bool              m_isClosing = false;
  Tnote             m_note;
};

#endif // TBANDONEONBG_H
