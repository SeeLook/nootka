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


#ifndef TGOTIT_H
#define TGOTIT_H

#include <QtCore/qobject.h>


class TgotIt : public QObject
{

  Q_OBJECT

  Q_PROPERTY(EgotItType gotItType READ gotItType WRITE setGotItType NOTIFY gotItTypeChanged)

public:
  explicit TgotIt(QObject* parent = nullptr);
    ~TgotIt() override;

  enum EgotItType {
    GotAnything = 0,
    GotSoundInfo,
    GotExamOrExer
  };
  Q_ENUM(EgotItType)

  EgotItType gotItType() const { return m_gotItType; }
  void setGotItType(EgotItType gt);

signals:
  void gotItTypeChanged();


private:
  EgotItType                     m_gotItType = GotAnything;

};

#endif // TGOTIT_H
