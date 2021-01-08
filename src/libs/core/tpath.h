/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#ifndef TPATH_H
#define TPATH_H


#include "nootkacoreglobal.h"
#include <QtCore/qstring.h>


/**
 * Set of static methods returning appropriate paths
 * @p main - this is default path in Nootka -
 * it is initialized at the launching begin.
 * -- Linux: nootka_executable_path/../usr/share
 * -- MacOs: /Resources folder in app bundle
 * -- Windows: the same as executable
 * -- Android: Qt resources system is used, so :/
 * but some files (i.e sounds, translations) are in assets: of apk
 *
 * @p img() for images
 * @p sound() for sounds
 * @p lang() for translations
*/
class NOOTKACORE_EXPORT Tpath
{

public:
  Tpath() {}
  ~Tpath() {}

  static QString main; /**< Path with Nootka resources (/usr/share -Linux /Resources - MacOs) */

      /**
       * Returns path to Nootka images (picts) with given image name.
       * By default a '.png' extensions is added
       * but it can be changed through @p ext parameter.
       */
  static QString img(const char* imageFileName, const char* ext = ".png");

      /**
       * Returns path to Nootka images (picts) with given image name.
       * It is accessible from QML through @p Noo.pix("image_name")
       */
  static QString pix(const QString& imageFileName);


      /**
       * Returns a path to given ogg file with samples in sound resource directory
       */
  static QString sound(const char* soundFileName, const char* ext = ".ogg") {
#if defined (Q_OS_ANDROID)
      return QString("assets:/sounds/%1%2").arg(soundFileName, ext);
#else
      return QString("%1sounds/%2%3").arg(main, soundFileName, ext);
#endif
  }

      /**
       * Returns path where Nootka language files (*.qm) are located.
       * (Under Mac/Win/Android Qt translations as well).
       * WARNING! there is no trailing slash at the end.
       */
  static QString lang() {
#if defined (Q_OS_ANDROID)
      return QStringLiteral("assets:/lang");
#else
      return main + QLatin1String("lang");
#endif
  }

        /**
         * Path with defined Nootka levels *.nel files
         */
  static QString levels() {
#if defined (Q_OS_ANDROID)
    return QStringLiteral("assets:/levels");
#else
    return main + QLatin1String("levels");
#endif
  }

};



#endif // TPATH_H
