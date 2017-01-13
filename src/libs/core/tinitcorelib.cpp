/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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

#include "tinitcorelib.h"
#include "music/ttune.h"
#include "music/tkeysignature.h"
#include "music/tclef.h"
#include "tcolor.h"
#include "tscoreparams.h"
#include "tpath.h"
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qtranslator.h>
#include <QtCore/qlibraryinfo.h>
#include <QtGui/qfontdatabase.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtQml/qqmlengine.h>
#if defined (Q_OS_ANDROID)
  #include "Android/tandroid.h"
  #include <QtWidgets/qstylefactory.h>
#endif


Tglobals* Tcore::m_gl = nullptr;

#if defined (Q_OS_ANDROID)
  QStyle* Tcore::androidStyle = nullptr;
#endif

bool initCoreLibrary() {
  if (Tcore::gl() == nullptr) {
    qDebug() << "Tglobals was not created. Construct it first!";
    return false;
  }

  qmlRegisterUncreatableType<Tclef>("Score", 1, 0, "Tclef", "You cannot create an instance of the Tclef.");

#if defined (Q_OS_ANDROID)
  if (Tcore::androidStyle == nullptr)
    Tcore::androidStyle = QStyleFactory::create(QStringLiteral("android"));
#endif

  Tcolor::setShadow(qApp->palette());
#if defined(Q_OS_MAC)
  QDir dir(qApp->applicationDirPath());
  dir.cdUp();
  qApp->addLibraryPath(dir.path() + QLatin1String("/Frameworks"));
#else
  qApp->addLibraryPath(qApp->applicationDirPath());
#endif

#if defined(Q_OS_LINUX)
  QDir dir(qApp->applicationDirPath());
  dir.cdUp();
  qApp->addLibraryPath(dir.path() + QLatin1String("/lib/nootka"));
#endif
#if defined (Q_OS_ANDROID)
  qApp->addLibraryPath(qApp->applicationDirPath());
  Tandroid::setScreenLockDisabled(); // TODO: interact with some settings option
#endif

  return true;
}


void prepareTranslations(QApplication* a, QTranslator& qt, QTranslator& noo) {
  if (!Tcore::gl())
    return;

#if defined (Q_OS_ANDROID)
  QLocale loc(Tcore::gl()->lang.isEmpty() ? QLocale::system() : Tcore::gl()->lang);
#elif defined (Q_OS_MAC)
  QLocale loc(Tcore::gl()->lang.isEmpty() ? QLocale::system().uiLanguages().first() : Tcore::gl()->lang);
#else
  QLocale loc(Tcore::gl()->lang.isEmpty() ? qgetenv("LANG") : Tcore::gl()->lang);
#endif
  QLocale::setDefault(loc);

  QString translationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#if !defined (Q_OS_LINUX) || defined (Q_OS_ANDROID)
  translationsPath = Tpath::lang();
#endif

  if (loc.language() == QLocale::Spanish) // So far, there are missing
    translationsPath = Tpath::lang(); // TODO Check when those qtbase translations will be shipped with Qt
  if (qt.load(loc, QStringLiteral("qtbase_"), QString(), translationsPath))
    a->installTranslator(&qt);

#if defined (Q_OS_ANDROID)
  // Try to load Nootka translation from user available location to give translators a way to check their work
  if (!noo.load(loc, QStringLiteral("nootka_"), QString(), Tandroid::getExternalPath() + "/Nootka"))
  // if there is not - look for it in standard path
#endif
  noo.load(loc, QStringLiteral("nootka_"), QString(), Tpath::lang());
  a->installTranslator(&noo);

  TkeySignature::setNameStyle(Tcore::gl()->S->nameStyleInKeySign, Tcore::gl()->S->majKeyNameSufix,
                              Tcore::gl()->S->minKeyNameSufix);
  Ttune::prepareDefinedTunes();
}


bool loadNootkaFont(QApplication* a) {
    QFontDatabase fd;
  int fid = fd.addApplicationFont(Tpath::main + QLatin1String("fonts/nootka.ttf"));
  int fid2 = fd.addApplicationFont(Tpath::main + QLatin1String("fonts/Scorek.otf"));
  if (fid == -1 || fid2 == -1) {
      QMessageBox::critical(0, QString(), a->translate("main", "<center>Can not load a font.<br>Try to install nootka.ttf manually.</center>"));
      return false;
  }
  return true;
}



