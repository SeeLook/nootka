/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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
#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <tinitcorelib.h>
#include "nootiniwindow.h"


Tglobals *gl;

int main(int argc, char *argv[])
{
  QTranslator qtTranslator;
  QTranslator nooTranslator;
  QApplication a(argc, argv);
  a.setApplicationName("Nootini");
  gl = new Tglobals(); // load configuration from temp file
  gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
  if (!initCoreLibrary())
    return 110;
  prepareTranslations(&a, qtTranslator, nooTranslator);
  if (!loadNootkaFont(&a))
    return 111;

  QString fName = "";
  if (a.arguments().size() > 1)
    fName = a.arguments()[1];
  NootiniWindow nw(fName);
  nw.show();

  int exitCode = a.exec();

  delete gl;
  return exitCode;
}
