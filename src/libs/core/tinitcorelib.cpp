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

#include "tinitcorelib.h"
#include "music/ttune.h"
#include "music/tkeysignature.h"
#include "widgets/tpushbutton.h"
#include "tcolor.h"
#include <QApplication>

Tglobals* Tglob::m_gl = 0;


void initCoreLibrary(Tglobals* gl) {
		Tglob::setGlobals(gl);
		Ttune::prepareDefinedTunes();
		Tcolor::setShadow(qApp->palette());
#if defined(Q_OS_MAC)
    TpushButton::setCheckColor(gl->SpointerColor, qApp->palette().base().color());
#else
    TpushButton::setCheckColor(qApp->palette().highlight().color(), qApp->palette().highlightedText().color() );
#endif
		TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
}


