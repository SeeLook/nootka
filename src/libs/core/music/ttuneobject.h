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
 * It is used only by @p Tglobals class.
 * It also tracks tuning type for scordature.
 */
class NOOTKACORE_EXPORT TtuneObject : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name NOTIFY tuningChanged)
  Q_PROPERTY(int type READ typeInt NOTIFY tuningChanged)
  Q_PROPERTY(int stringNumber READ stringNumber NOTIFY tuningChanged)
  Q_PROPERTY(bool scordature READ scordature NOTIFY scordatureChanged)

public:

  explicit TtuneObject(QObject* parent = nullptr);
  ~TtuneObject() override;

  void setTune(Ttune* t);

  QString name() const;

  int typeInt() const { return static_cast<int>(m_tuning->type()); }

  Q_INVOKABLE Tnote string(int realStrNr) const { return m_tuning->str(realStrNr); }

      /**
       * Name of a given string (note name without octave)
       */
  Q_INVOKABLE QString stringName(int realStrNr) const;

      /**
       * Returns @p TRUE when given string is different than the same string in standard guitar tuning
       */
  Q_INVOKABLE bool otherThanStd(int realStrNr) const;

  int stringNumber() { return static_cast<int>(m_tuning->stringNr()); }

      /**
       * @p TRUE when tuning is guitar tuning (not bass) and differs from standard EADGBE one
       */
  bool scordature() const;

      /**
       * Number of strings that were changed (tuned)
       */
  Q_INVOKABLE int changedStrings() const;

      /**
       * Raw copy of the tuning (rather CPU expensive)
       */
  Q_INVOKABLE Ttune raw() { return *m_tuning; }

signals:
  void tuningChanged();
  void scordatureChanged();

private:
  Ttune                 *m_tuning;
};


#endif // TTUNEOBJECT_H
