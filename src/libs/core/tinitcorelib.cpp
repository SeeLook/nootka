/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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
#include "tglobals.h"
#include "music/ttune.h"
#include "music/tkeysignature.h"
#include "tcolor.h"
#include "tscoreparams.h"
#include "tpath.h"
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qtranslator.h>
#include <QtCore/qlibraryinfo.h>
#include <QtGui/qfontdatabase.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#if defined (Q_OS_ANDROID)
  #include "Android/tandroid.h"
#endif


bool initCoreLibrary() {
  if (GLOB == nullptr) {
    qDebug() << "Tglobals was not created. Construct it first!";
    return false;
  }

  Trhythm::initialize();
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
  Tandroid::keepScreenOn(GLOB->isKeepScreenOn());
  Tandroid::disableRotation(GLOB->disableRotation());
#endif

  return true;
}


void prepareTranslations(QGuiApplication* a, QTranslator& qt, QTranslator& noo) {
  if (!GLOB)
    return;

#if defined (Q_OS_ANDROID)
  QLocale loc(GLOB->lang.isEmpty() ? QLocale::system() : GLOB->lang);
#elif defined (Q_OS_MAC)
  QLocale loc(GLOB->lang.isEmpty() ? QLocale::system().uiLanguages().first() : GLOB->lang);
#else
  QLocale loc(GLOB->lang.isEmpty() ? qgetenv("LANG") : GLOB->lang);
#endif
  QLocale::setDefault(loc);

  QString translationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#if !defined (Q_OS_LINUX) || defined (Q_OS_ANDROID)
  translationsPath = Tpath::lang();
#endif

  if (qt.load(loc, QStringLiteral("qtbase_"), QString(), translationsPath))
    a->installTranslator(&qt);

#if defined (Q_OS_ANDROID)
  // Try to load Nootka translation from user available location to give translators a way to check their work
  if (!noo.load(loc, QStringLiteral("nootka_"), QString(), Tandroid::getExternalPath() + "/Nootka"))
  // if there is not - look for it in standard path
#endif
  noo.load(loc, QStringLiteral("nootka_"), QString(), Tpath::lang());
  a->installTranslator(&noo);

  TkeySignature::setNameStyle(GLOB->S->nameStyleInKeySign, GLOB->S->majKeyNameSufix,
                              GLOB->S->minKeyNameSufix);
  Ttune::prepareDefinedTunes();
}


bool loadNootkaFont(QGuiApplication* a) {
  Q_UNUSED(a)
  QFontDatabase fd;
  int fid = fd.addApplicationFont(Tpath::main + QLatin1String("fonts/nootka.ttf"));
  int fid2 = fd.addApplicationFont(Tpath::main + QLatin1String("fonts/Scorek.otf"));
  if (fid == -1 || fid2 == -1) {
    qDebug() << "Can not load Nootka fonts!\nDid you forget to invoke\n  make runinplace\nafter first compilation?\n";
    return false;
  }
  return true;
}



