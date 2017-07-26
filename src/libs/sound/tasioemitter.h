/***************************************************************************
 *   Copyright (C) 2015-2017 by Tomasz Bojczuk                             *
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



#ifndef TASIOEMITTER_H
#define TASIOEMITTER_H


#include <QtCore/qobject.h>


/**
 * This is @p QObject that emits @p resetASIO() signal
 * @p RtApiAsio has static instance of it
 * end emits this signal when ASIO requires restart.
 */
class TASIOEmitter : public QObject
{
  Q_OBJECT

public:
  explicit TASIOEmitter(QObject* parent = 0) : QObject(parent) {}

  void emitResetASIO() { emit resetASIO(); }

signals:

      /**
       * Emitted when user changes devices or parameters in ASIO console - stream have to be stopped and started again
       */
  void resetASIO();

};

#endif // TASIOEMITTER_H
