/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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


#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
// #include <QtGui/QPixmapCache>
#include <tinitcorelib.h>
#include "declarative/denote.h"
#include "declarative/descore.h"


Tglobals *gl;

int main(int argc, char *argv[])
{
  QTranslator qtTranslator;
  QTranslator nooTranslator;
//   QPixmapCache::setCacheLimit(102400);
  QApplication a(argc, argv);
  gl = new Tglobals(); // load configuration from temp file
  gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
  if (!initCoreLibrary())
    return 110;
  prepareTranslations(&a, qtTranslator, nooTranslator);
  if (!loadNootkaFont(&a))
    return 111;
  a.setApplicationName("Scorek");

  qmlRegisterType<DeScore>("Score", 1, 0, "Score");
  qmlRegisterType<DeNote>("Note", 1, 0, "Note");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));

  int exitCode = a.exec();

  delete gl;
  return exitCode;
}
