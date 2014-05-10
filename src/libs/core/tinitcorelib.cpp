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
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFontDatabase>
#include <QDebug>


Tglobals* Tglob::m_gl = 0;
QTranslator qtTranslator;
QTranslator nooTranslator;


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


void prepareTranslations(QApplication* a) {
	QString ll = "";
	if (Tglob::glob())
	ll = Tglob::glob()->lang;
	if (ll == "")
			ll = QLocale::system().name();
// 	QTranslator qtTranslator;
#if defined(Q_OS_LINUX)
    qtTranslator.load("qt_" + ll, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#else
    qtTranslator.load("qt_" + ll, Tglob::glob()->path + "lang");
#endif
	a->installTranslator(&qtTranslator);

//     QTranslator nooTranslator;
	nooTranslator.load("nootka_" + ll, Tglob::glob()->path + "lang");
	a->installTranslator(&nooTranslator);
}


bool loadNootkaFont(QApplication* a) {
	QFontDatabase fd;
	int fid = fd.addApplicationFont(Tglob::glob()->path + "fonts/nootka.ttf");
	if (fid == -1) {
            QMessageBox::critical(0, "", a->translate("main", "<center>Can not load a font.<br>Try to install nootka.ttf manually.</center>"));
			return false;
	} else
			return true;
}



