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

#include <nootkacoreglobal.h>
#include "music/ttune.h"
#include "music/tkeysignature.h"
#include "widgets/tpushbutton.h"
#include "tglobals.h"
#include "tcolor.h"


/** Initializes static values in library:
 * - tuning definitions
 * - TpushButton colors
 */
NOOTKACORE_EXPORT void initCoreLibrary(QWidget *anyW, Tglobals *gl) {
		Ttune::prepareDefinedTunes();
		Tcolor::setShadow(anyW);
#if defined(Q_OS_MAC)
    TpushButton::setCheckColor(gl->SpointerColor, anyW->palette().base().color());
#else
    TpushButton::setCheckColor(anyW->palette().highlight().color(), anyW->palette().highlightedText().color() );
#endif
		TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
}




#endif // TINITCORELIB_H
