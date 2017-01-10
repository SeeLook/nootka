/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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


#if defined (Q_OS_ANDROID)
  #include "ttouchstyle.h"
  #include <Android/tandroid.h>
#endif
#include <tinitcorelib.h>
#include <tpath.h>
#include <tmtr.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qicon.h>
#include <QtQml/qqmlapplicationengine.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtranslator.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpointer.h>
#include <QtCore/qfile.h>
#include <QtCore/qsettings.h>


static QString logFile;

/** It allows to grab all debug messages into nootka-log.txt file */
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  Q_UNUSED(context)
  Q_UNUSED(type)
//   if (type == QtDebugMsg) {
#if defined (Q_OS_ANDROID)
    QFile outFile(logFile);
#else
    QFile outFile(QStringLiteral("nootka-log.txt"));
#endif
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << msg << endl;
//   }
}


Tglobals *gl;
bool resetConfig;


int main(int argc, char *argv[])
{
#if defined (Q_OS_ANDROID)
  qputenv("QT_ANDROID_VOLUME_KEYS", "1"); // Handle volume keys by Qt, lock native Android behavior

  // log to any writable storage
  logFile = Tandroid::getExternalPath() + QStringLiteral("/nootka-log.txt");
  if (QFile::exists(logFile))
    QFile::remove(logFile);
  qInstallMessageHandler(myMessageOutput);
  qDebug() << "==== NOOTKA LOG =======\n" << QDateTime::currentDateTime().toString();
#endif

  QTranslator qtTranslator;
  QTranslator nooTranslator;
  QPointer<QApplication> a = 0;
  QQmlApplicationEngine *e = nullptr;

  int exitCode;
  bool firstTime = true;
  QString confFile;
  resetConfig = false;
  do {
#if !defined (Q_OS_ANDROID)
    if (a) delete a;
    if (resetConfig) { // delete config file - new Nootka instance will start with first run wizard
        QFile f(confFile);
        f.remove();
    }
    resetConfig = false;
#endif
    a = new QApplication(argc, argv);
    Tmtr::init(a);

    gl = new Tglobals();
    gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
    confFile = gl->config->fileName();
    if (!initCoreLibrary())
      return 110;
    prepareTranslations(a, qtTranslator, nooTranslator);
    if (!loadNootkaFont(a))
      return 111;

    a->setWindowIcon(QIcon(Tpath::img("nootka")));
// creating main window
    e = new QQmlApplicationEngine(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

// #if defined (Q_OS_ANDROID)
//     w->showFullScreen(); // TODO
// #endif

    if (firstTime) {
#if defined (Q_OS_ANDROID)
      QString androidArg = Tandroid::getRunArgument();
      if (!androidArg.isEmpty())
        w->openFile(androidArg);
#else // TODO
//       if (argc > 1)
//         w->openFile(QString::fromLocal8Bit(argv[argc - 1]));
#endif
    }
    firstTime = false;
    exitCode = a->exec();
    delete e;
    delete gl;
#if defined (Q_OS_ANDROID)
    if (resetConfig) { // delete config file - new Nootka instance will start with first run wizard
        QFile f(confFile);
        f.remove();
        Tandroid::restartNootka(); // and call Nootka after delay
    }
    resetConfig = false; // do - while loop doesn't work with Android
    qApp->quit(); // HACK: calling QApplication::quick() solves hang on x86 when Qt uses native (usually obsolete) SSL libraries
#endif
  } while (resetConfig);

  delete a;
  return exitCode;
}
