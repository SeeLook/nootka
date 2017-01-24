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

#ifndef TNOOTKAQML_H
#define TNOOTKAQML_H


#include <QtCore/qobject.h>
#include "music/tclef.h"


/**
 * Singleton object to manage (create) custom types from QML
 * In constructor it registers types accessible from QML in Nootka
 */
class TnootkaQML : public QObject
{

  Q_OBJECT

public:
  explicit TnootkaQML(QObject* parent = nullptr);
  ~TnootkaQML();


  Q_INVOKABLE QString version();
  Q_INVOKABLE Tclef clef(int type);

private:
  static TnootkaQML             *m_instance;
};

#endif // TNOOTKAQML_H
