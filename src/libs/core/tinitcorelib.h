/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TINITCORELIB_H
#define TINITCORELIB_H

#include "nootkacoreglobal.h"
#include "tglobals.h"

class QTranslator;
class QApplication;

/** 
 * Internal instance of Tglobals pointer used by in initCoreLibrary. *
 * It is set during invoking initCoreLibrary()
 */
class NOOTKACORE_EXPORT Tcore
{

public:
	static void setGlobals(Tglobals *g) { m_gl = g; }
	static Tglobals* gl() { return m_gl; } /** static global pointer to Tglobals */
	
private:
	static Tglobals *m_gl; 

};

/** Initializes static values in library:
 * - pointer to Tglobals initialized by external executable !!!
 *   it is accessible through @p Tcore::gl() method 
 * - tuning definitions
 * - TpushButton colors
 */
NOOTKACORE_EXPORT void initCoreLibrary(Tglobals *gl);

/** Loads translations files for appropriate language (system or user preferred)
 * Translator object has to be created first. */
NOOTKACORE_EXPORT void prepareTranslations(QApplication* a, QTranslator& qt, QTranslator& qtBase, QTranslator& noo);

/** Checks nootka.ttf file and loads it. Returns true if successful.  
 * libNootkaCore has to be initialized first by initCoreLibrary() */
NOOTKACORE_EXPORT bool loadNootkaFont(QApplication* a);

#endif // TINITCORELIB_H
