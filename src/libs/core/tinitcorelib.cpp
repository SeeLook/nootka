/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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
#include "tscoreparams.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFontDatabase>
#include <QDebug>
#include <QDir>


Tglobals* Tcore::m_gl = 0;


bool initCoreLibrary() {
	if (Tcore::gl() == 0) {
		qDebug() << "Tglobals was not created. Construct it first!";
    return false;
	}
	Tcolor::setShadow(qApp->palette());
#if defined(Q_OS_MAC)
	TpushButton::setCheckColor(Tcore::gl()->S->pointerColor, qApp->palette().base().color());
#else
	TpushButton::setCheckColor(qApp->palette().highlight().color(), qApp->palette().highlightedText().color() );
  qApp->addLibraryPath(qApp->applicationDirPath());
#endif

#if defined(Q_OS_LINUX)
  QDir dir(qApp->applicationDirPath());
  dir.cdUp();
  qApp->addLibraryPath(dir.path() + QStringLiteral("/lib/nootka"));
#endif

#if defined(Q_OS_MAC)
  QDir dir(qApp->applicationDirPath());
  dir.cdUp();
  qApp->addLibraryPath(dir.path() + QStringLiteral("/Frameworks"));
#endif
  return true;
}


void prepareTranslations(QApplication* a, QTranslator& qt, QTranslator& noo) {
	if (!Tcore::gl())
    return;

#if defined (Q_OS_MAC)
  QLocale loc(Tcore::gl()->lang.isEmpty() ? QLocale::system().uiLanguages().first() : Tcore::gl()->lang);
#else
  QLocale loc(Tcore::gl()->lang.isEmpty() ? qgetenv("LANG") : Tcore::gl()->lang);
#endif
  QLocale::setDefault(loc);

	QString translationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#if !defined (Q_OS_LINUX)
  translationsPath = Tcore::gl()->path + "lang";
#endif
  /** Until Qt 5.2 version translations where inside qt_xx.ts files
   * and all shipped with Qt for all supported languages.
   * But since Qt 5.3 they are split into several files and Nootka requires just qtbase_xx.ts.
   * qtbase_pl.ts and qtbase_fr.ts are missing so far but they was obtained and shipped with Nootka. */
  QString qtlang = "qtbase_";
#if (QT_VERSION < QT_VERSION_CHECK(5, 3, 0))
  qtlang = "qt_";
#else
  if ( loc.country() == QLocale::Poland || loc.country() == QLocale::France) // So far, there are missing
    translationsPath = Tcore::gl()->path + "lang"; // TODO Check when those qtbase translations will be shipped with Qt
#endif
  if (qt.load(loc, qtlang, "", translationsPath))
    a->installTranslator(&qt);
  noo.load(loc, "nootka_", "", Tcore::gl()->path + "lang");
  a->installTranslator(&noo);

	TkeySignature::setNameStyle(Tcore::gl()->S->nameStyleInKeySign, Tcore::gl()->S->majKeyNameSufix, 
															Tcore::gl()->S->minKeyNameSufix);
  Ttune::prepareDefinedTunes();
}


bool loadNootkaFont(QApplication* a) {
    QFontDatabase fd;
	int fid = fd.addApplicationFont(Tcore::gl()->path + "fonts/nootka.ttf");
	if (fid == -1) {
            QMessageBox::critical(0, "", a->translate("main", "<center>Can not load a font.<br>Try to install nootka.ttf manually.</center>"));
			return false;
	}
	return true;
}



