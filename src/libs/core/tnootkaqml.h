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


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


class Tclef;
class Tmeter;


/**
 * Singleton object to manage (create) custom types from QML
 * In constructor it registers types accessible from QML in Nootka
 */
class NOOTKACORE_EXPORT TnootkaQML : public QObject
{

  Q_OBJECT

public:
  explicit TnootkaQML(QObject* parent = nullptr);
  ~TnootkaQML();


  Q_INVOKABLE QString version();
  Q_INVOKABLE Tclef clef(int type);
  Q_INVOKABLE Tmeter meter(int m);
  Q_INVOKABLE QString majorKeyName(int key);
  Q_INVOKABLE QString minorKeyName(int key);
  Q_INVOKABLE QString getLicense();

private:
  static TnootkaQML             *m_instance;
};

#endif // TNOOTKAQML_H
