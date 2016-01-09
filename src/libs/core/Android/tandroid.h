/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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


#ifndef TANDROID_H
#define TANDROID_H

#include <QtCore/qstring.h>

/**
 * Android functions requiring invoking native methods through JNI
 */
namespace Tandroid {

  void setScreenLockDisabled(); /**< Sets phone/tablet screen saving disabled. */
  QString getExternalPath(); /**< Returns path to external storage (SD card). */
  QString getRunArgument(); /**< Returns command line argument (usually exam/level file name. */
  void sendExam(const QString& title, const QString& message, const QString& filePath);
  QString accountName();

}


#endif // TANDROID_H
