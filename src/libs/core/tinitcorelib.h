/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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
#include "tglobals.h"

class QTranslator;
class QApplication;

/** 
 * Internal instance of Tglobals pointer used by in initCoreLibrary. *
 * It should be set during first call of @p Tglobals constructor
 */
class NOOTKACORE_EXPORT Tcore
{

	friend class Tglobals;
	
public:
	static Tglobals* gl() { return m_gl; } /** static global pointer to Tglobals */
	
protected:
	static void setGlobals(Tglobals *g) { m_gl = g; }
	static void reset() { m_gl = 0; } /** Set pointer to @p Tglobals to 0 */
	
private:
	static Tglobals *m_gl; 

};


/** Initializes static values in library:
 * - checks is @p Tglobal instance exists (@p Tcore::setGlobals() was called)
 * - adds tuning definitions
 * - adds TpushButton colors
 */
NOOTKACORE_EXPORT bool initCoreLibrary();

/** Loads translations files for appropriate language (system or user preferred)
 * Translator object has to be created before. */
NOOTKACORE_EXPORT void prepareTranslations(QApplication* a, QTranslator& qt, QTranslator& noo);

/** Checks nootka.ttf file and loads it. Returns true if successful.  
 * libNootkaCore has to be initialized first by initCoreLibrary() */
NOOTKACORE_EXPORT bool loadNootkaFont(QApplication* a);

#endif // TINITCORELIB_H
