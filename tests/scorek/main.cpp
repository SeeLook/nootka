/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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


#include <QtCore/qtranslator.h>
#include <QtCore/QDir>
#include <QtCore/qdebug.h>
#include <QtWidgets/qapplication.h>
#include <tinitcorelib.h>
#include <scorekwindow.h>


Tglobals *gl;

int main(int argc, char *argv[])
{
  QTranslator qtTranslator;
  QTranslator nooTranslator;
  QApplication a(argc, argv);
  gl = new Tglobals(); // load configuration from temp file
  gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
  if (!initCoreLibrary())
    return 110;
  prepareTranslations(&a, qtTranslator, nooTranslator);
  if (!loadNootkaFont(&a))
    return 111;
  a.setApplicationName("Scorek");

  ScorekWindow sk;
  sk.show();

  int exitCode = a.exec();

  delete gl;
  return exitCode;
}
