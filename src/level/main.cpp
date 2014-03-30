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


#include <QApplication>
#include "tlevelcreatordlg.h"
#include <tinitcorelib.h>

Tglobals *gl;

int main(int argc, char *argv[])
{    	
		QApplication a(argc, argv);
// #if defined (Q_OS_MAC)
// 		QApplication::setStyle(new QPlastiqueStyle);
// #endif
		gl = new Tglobals();
		gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
		initCoreLibrary(gl);
		prepareTranslations(&a);
		if (!loadNootkaFont(&a))
			return 111;

    TlevelCreatorDlg creator;
    creator.show();
		return a.exec();
}
