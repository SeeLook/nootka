/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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
#include <QtCore/qdir.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qdebug.h>
#include <QtWidgets/qapplication.h>
#include <tinitcorelib.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <plugins/tpluginsloader.h>
#include <qt/QtWidgets/qmainwindow.h>
#include <qt/QtWidgets/QLabel>


Tglobals *gl;

int main(int argc, char *argv[])
{
  if (argc < 2) {
    qDebug() << "Usage:\nnootest <plugin name[Level|Settings|Analyzer|Updater|Wizard|About]> [argument(s), leve/exam filename]";
    return 0;
  }
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

// Determine plugin type
  TpluginsLoader::Etype plugType;
  if (a.arguments()[1] == "Level")
    plugType = TpluginsLoader::e_level;
  else if (a.arguments()[1] == "Settings")
    plugType = TpluginsLoader::e_settings;
  else if (a.arguments()[1] == "Analyzer")
    plugType = TpluginsLoader::e_analyzer;
  else if (a.arguments()[1] == "Updater")
    plugType = TpluginsLoader::e_updater;
  else if (a.arguments()[1] == "Wizard")
    plugType = TpluginsLoader::e_wizard;
  else if (a.arguments()[1] == "About")
    plugType = TpluginsLoader::e_about;
  else {
    qDebug() << "Unrecognized plugin type:" << a.arguments()[1];
    return 1;
  }

// Prepare additional plugin parameters
  Tlevel *level = 0;
  Texam *exam = 0;
  bool plugOk = false;
  TpluginsLoader *loader = new TpluginsLoader(0);
  QString pluginArgument;
  if (a.arguments().size() > 2)
    pluginArgument = a.arguments()[2];
  if (!pluginArgument.isEmpty() && plugType == TpluginsLoader::e_analyzer) { // create exam file
    level = new Tlevel();
    exam = new Texam(level, QString());
    Texam::EerrorType err = exam->loadFromFile(pluginArgument);
      if (!(err == Texam::e_file_OK || err == Texam::e_file_corrupted)) {
        qDebug() << "Can not read from exam file" << pluginArgument << (int)err;
        delete exam;
        exam = 0;
        delete level;
        level = 0;
      }
  }

// Load a plugin and initialize it
  if (loader->load(plugType)) {
    plugOk = loader->init(pluginArgument, 0, exam);
  }

  if (plugOk) {
    if (plugType == TpluginsLoader::e_updater) {
			QMainWindow w;
			QLabel *l = new QLabel(&w);
			w.setCentralWidget(l);
			a.connect(loader->node(), SIGNAL(message(QString)), l, SLOT(setText(QString)));
			w.show();
      a.exec();
		}
    qDebug() << "plugin said:" << loader->lastWord();
  }
//   delete loader;
  if (exam) delete exam;
  if (level) delete level;
  delete gl;
  return 0;
}
