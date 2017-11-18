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

#ifndef TDIALOGLOADEROBJECT_H
#define TDIALOGLOADEROBJECT_H

#include <QtCore/qobject.h>

/**
 * Implements logic for dialogues loader (@p DialogLoader.qml),
 * but also initializes C++ components related to exams/levels for QML.
 * Those components are not necessary when Nootka starts
 */
class TdialogLoaderObject : public QObject
{

  Q_OBJECT

public:
  TdialogLoaderObject(QObject* parent = nullptr);
  ~TdialogLoaderObject() override;

      /**
       * Returns translated text according to Qt @p StandartButtons enumerator
       */
  Q_INVOKABLE QString stdButtonText(int role);

      /**
       * Icon image name of given StandartButtons role
       */
  Q_INVOKABLE QString stdButtonIcon(int role);

private:
  bool                    m_firstTime = true;

};

#endif // TDIALOGLOADEROBJECT_H
