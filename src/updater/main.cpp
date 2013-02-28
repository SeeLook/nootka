/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include <QtGui>
#include "tupdatechecker.h"

// #include "tglobals.h"
// 
// Tglobals *gl;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Nootka");
    QCoreApplication::setOrganizationDomain("nootka.sf.net");
    QCoreApplication::setApplicationName("Nootka");
  
    QString path;
    QDir d = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
    path = d.path() + "/";
#elif defined(Q_OS_LINUX)
    d.cdUp();
    path = d.path() + "/share/nootka/";  // Linux
    QSettings settings;
#else
    QSettings settings;
    d.cdUp();
    path = d.path() + "/Resources/";     // MacOs
#endif
    settings.beginGroup("common");
      QString ll = settings.value("language", "").toString();
    settings.endGroup();
    if (ll == "") ll = QLocale::system().name();

    QTranslator nooTranslator;
    nooTranslator.load("nootka_" + ll, path + "lang");
    a.installTranslator(&nooTranslator);
  
//    if (argc > 1)
//      TupdateChecker u(true);
//    else
//      TupdateChecker u(false);
    TupdateChecker u(false);

    return a.exec();
}
