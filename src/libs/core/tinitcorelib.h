/***************************************************************************
 *   Copyright (C) 2014-2019 by Tomasz Bojczuk                             *
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

#ifndef TINITCORELIB_H
#define TINITCORELIB_H

#include "nootkacoreglobal.h"


class QTranslator;
class QGuiApplication;


    /**
     * Initializes static values in library:
     * - adds tuning definitions
     * - adds TpushButton colors
     */
NOOTKACORE_EXPORT bool initCoreLibrary();

    /**
     * Loads translations files for appropriate language (system or user preferred)
     * Translator object has to be created before.
     */
NOOTKACORE_EXPORT void prepareTranslations(QGuiApplication* a, QTranslator& qt, QTranslator& noo);

    /**
     * Checks nootka.ttf file and loads it. Returns true if successful.
     * libNootkaCore has to be initialized first by initCoreLibrary()
     */
NOOTKACORE_EXPORT bool loadNootkaFont(QGuiApplication* a);

#endif // TINITCORELIB_H
