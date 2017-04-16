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

#ifndef TTUNEOBJECT_H
#define TTUNEOBJECT_H


#include "nootkacoreglobal.h"
#include "ttune.h"
#include <QtCore/qobject.h>


/**
 * Wraps @class Ttune with @class QObject
 */
class NOOTKACORE_EXPORT TtuneObject : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name NOTIFY tuningChanged)
  Q_PROPERTY(Ttune::Etunings type READ type NOTIFY tuningChanged)
  Q_PROPERTY(int stringNumber READ stringNumber NOTIFY tuningChanged)

public:

  explicit TtuneObject(QObject* parent = nullptr);
  ~TtuneObject();

  void setTune(Ttune* t);

  QString name() const;

  Ttune::Etunings type() const { return m_tune->type(); }

  Q_INVOKABLE Tnote string(int realNr) const { return m_tune->str(realNr); }

  int stringNumber() { return static_cast<int>(m_tune->stringNr()); }

signals:
  void tuningChanged();

private:
  Ttune                 *m_tune;
};


#endif // TTUNEOBJECT_H
