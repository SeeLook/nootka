/***************************************************************************
 *   Copyright (C) 2014-2025 by Tomasz Bojczuk                             *
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
#include "music/tkeysignature.h"
#include "music/ttune.h"
#include "qtr.h"
#include "tcolor.h"
#include "tglobals.h"
#include "tpath.h"
#include "tscoreparams.h"
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qtranslator.h>
#include <QtGui/qfontdatabase.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#if defined(Q_OS_ANDROID)
#include "Android/tandroid.h"
#endif

bool initCoreLibrary()
{
    if (GLOB == nullptr) {
        qDebug() << "[tinitcorelib] Tglobals was not created. Construct it first!";
        return false;
    }

    // dummy - just to consume that by lupdate
    QGuiApplication::translate("about translator",
                               "translator",
                               "Do not translate this, just put in 'translator comment field' your data: translator name, "
                               "translator e-mail (optional) and translator site (optional)");

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
#if defined(Q_OS_ANDROID)
    qApp->addLibraryPath(qApp->applicationDirPath());
    Tandroid::keepScreenOn(GLOB->isKeepScreenOn());
    Tandroid::disableRotation(GLOB->disableRotation());
#endif

    return true;
}

void prepareTranslations(QGuiApplication *a, QTranslator &qt, QTranslator &noo)
{
    if (!GLOB)
        return;

#if defined(Q_OS_ANDROID)
    QLocale loc(GLOB->lang.isEmpty() ? QLocale::system().language() : QLocale(GLOB->lang).language());
#elif defined(Q_OS_MAC)
    QLocale loc(GLOB->lang.isEmpty() ? QLocale::system().uiLanguages().first() : GLOB->lang);
#elif defined(Q_OS_WIN)
    QLocale loc(GLOB->lang.isEmpty() ? QLocale::system().language() : QLocale(GLOB->lang).language());
#else
    QLocale loc(QLocale(GLOB->lang.isEmpty() ? qgetenv("LANG") : GLOB->lang).language(),
                QLocale(GLOB->lang.isEmpty() ? qgetenv("LANG") : GLOB->lang).territory());
#endif
    QLocale::setDefault(loc);

    QString translationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#if !defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    translationsPath = Tpath::lang();
#endif

    if (qt.load(loc, QStringLiteral("qtbase_"), QString(), translationsPath))
        a->installTranslator(&qt);

    if (noo.load(loc, QStringLiteral("nootka_"), QString(), Tpath::lang()))
        a->installTranslator(&noo);

    if (GLOB->isFirstRun) {
        GLOB->setSeventhIsB(qTR("Notation", "b").toLower() == QLatin1String("b"));
        if (GLOB->scoreParams->seventhIs_B)
            GLOB->scoreParams->nameStyleInNoteName = Tnote::e_english_Bb;
        else
            GLOB->scoreParams->nameStyleInNoteName = Tnote::e_norsk_Hb;
        if (qTR("Notation", "letters").toLower() == QLatin1String("solfege")) {
            if (loc.language() == QLocale::Russian)
                GLOB->scoreParams->nameStyleInNoteName = Tnote::e_russian_Ci;
            else
                GLOB->scoreParams->nameStyleInNoteName = Tnote::e_italiano_Si;
        }
        Tnote::defaultStyle = GLOB->scoreParams->nameStyleInNoteName;
    }

    TkeySignature::setNameStyle(GLOB->scoreParams->nameStyleInKeySign, GLOB->scoreParams->majKeyNameSufix, GLOB->scoreParams->minKeyNameSufix);
    Ttune::prepareDefinedTunes();
}

bool loadNootkaFont(QGuiApplication *a)
{
    Q_UNUSED(a)
    int fid = QFontDatabase::addApplicationFont(Tpath::main + QLatin1String("fonts/nootka.otf"));
    int fid2 = QFontDatabase::addApplicationFont(Tpath::main + QLatin1String("fonts/Scorek.otf"));
    if (fid == -1 || fid2 == -1) {
        qDebug() << "Cannot load Nootka fonts!\nDid you forget to invoke:\n"
                 << "make runinplace\nor\nninja runinplace\nafter the first compilation?\n";
        return false;
    }
    return true;
}
