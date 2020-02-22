/***************************************************************************
 *   Copyright (C) 2015-2020 by Tomasz Bojczuk                             *
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


#ifndef TANDROID_H
#define TANDROID_H


#include <QtCore/qstring.h>


/**
 * Android functions requiring invoking native methods through JNI
 */
namespace Tandroid {
  void keepScreenOn(bool on);

  void disableRotation(bool disRot);

      /**
       * Returns a number of Android API on a hosting device.
       */
  int getAPIlevelNr();

      /**
       * Returns path to external storage (SD card).
       */
  QString getExternalPath();

      /**
       * @p TRUE when write access to device is granted. API 23 and above
       */
  bool hasWriteAccess();

      /**
       * Returns command line argument (usually exam/level file name.
       */
  QString getRunArgument();

  void sendExam(const QString& title, const QString& message, const QString& filePath);

      /**
       * Restarts the application by calling java code with delay.
       * WARING! Application has to be terminated just after this method.
       */
  void restartNootka();

  QString accountName();

}


#endif // TANDROID_H
