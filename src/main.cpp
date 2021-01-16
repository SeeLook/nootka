/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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


#include <tinitcorelib.h>
#include <tglobals.h>
#include <tmtr.h>
#include <tpath.h>
#include <tnootkaqml.h>
#include <tsound.h>
#include "main/tnameitem.h"
#include "main/tmainscoreobject.h"
#include "dialogs/tdialogloaderobject.h"
#include "help/tmainhelp.h"

#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
  #include "mobile/tmobilemenu.h"
#else
  #include "nootini/taudioanalyzeitem.h"
#endif

#include <QtWidgets/qapplication.h>
#include <QtGui/qicon.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtCore/qtranslator.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpointer.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <QtCore/qsettings.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qloggingcategory.h>

#include <QtCore/qdebug.h>


static QString logFile;


/**
 * It allows to grab all debug messages into nootka-log.txt file
 */
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  Q_UNUSED(context)
  Q_UNUSED(type)

#if defined (Q_OS_ANDROID)
    QFile outFile(logFile);
#else
    QFile outFile(QStringLiteral("nootka-log.txt"));
#endif
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << msg << "\n";
}


int main(int argc, char *argv[])
{
#if defined (Q_OS_WIN)
  // It works under Win
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  // It mutes QML warnings about connections syntax introduced in Qt 5.15
  // TODO when Qt version requirements will rise to 5.15 or above, change syntax and remove that
  QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.connections=false"));

#if defined (Q_OS_ANDROID)
  qputenv("QT_ANDROID_VOLUME_KEYS", "1"); // Handle volume keys by Qt, lock native Android behavior

  // log to any writable storage
 logFile = Tandroid::getExternalPath() + QStringLiteral("/nootka-log.txt");
  if (QFile::exists(logFile))
    QFile::remove(logFile);
  qInstallMessageHandler(myMessageOutput);
  qDebug() << "==== NOOTKA LOG =======\n" << QDateTime::currentDateTime().toString();
#else
  qputenv("QT_QUICK_CONTROLS_STYLE", ""); // reset style environment var - other styles can cause crashes
#endif

  QElapsedTimer startElapsed;
  startElapsed.start();

  QTranslator qtTranslator;
  QTranslator nooTranslator;
  Tglobals *gl = nullptr;
  QPointer<QApplication> a = nullptr;
  QQmlApplicationEngine *e = nullptr;
  auto nooObj = new TnootkaQML();

  int exitCode;
  bool firstLoop = true;
  QString confFile;
  do {
#if !defined (Q_OS_ANDROID)
    if (a)
      delete a;
    if (nooObj->resetConfig()) { // delete config file - new Nootka instance will start with first run wizard
      QFile f(confFile);
      f.remove();
      auto exerciseFile = QDir::toNativeSeparators(QFileInfo(confFile).absolutePath() + QLatin1String("/exercise2.noo"));
      if (QFileInfo(exerciseFile).exists())
        QFile::remove(exerciseFile);
      // also clean exercise from Nootka versions up to 1.7.X
      exerciseFile = QDir::toNativeSeparators(QFileInfo(confFile).absolutePath() + QLatin1String("/exercise.noo"));
      if (QFileInfo(exerciseFile).exists())
        QFile::remove(exerciseFile);
    }
    nooObj->setResetConfig(false);
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

    auto f = a->font();
#if defined (Q_OS_ANDROID)
    f.setPixelSize(qRound(static_cast<qreal>(f.pixelSize()) * gl->guiScale()));
    auto pal = qApp->palette();
    pal.setColor(QPalette::Active, QPalette::Highlight, QColor(0, 160, 160)); // Teal color of highlight for Android
    pal.setColor(QPalette::Active, QPalette::Shadow, QColor(120, 120, 120)); // Dark gray for shadow
    pal.setColor(QPalette::Active, QPalette::Window, QColor(250, 250, 250)); // Almost white for windows
    pal.setColor(QPalette::Active, QPalette::Base, QColor(255, 255, 255)); // White base
    pal.setColor(QPalette::Active, QPalette::Button, QColor(240, 240, 240)); // Very light gray for button
    pal.setColor(QPalette::Active, QPalette::AlternateBase, QColor(245, 255, 255)); // Very light teal for alternate base
    qApp->setPalette(pal);
#elif defined (Q_OS_WIN)
    QSettings accent(QStringLiteral("HKEY_USERS\\.DEFAULT\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent"),
                     QSettings::NativeFormat);
    if (accent.contains(QLatin1String("StartColorMenu"))) {
      int color = accent.value(QLatin1String("StartColorMenu")).toInt();
      int r = color & 0xff;
      int g = (color >> 8) & 0xff;
      int b = (color >> 16) & 0xff;
      auto pal = qApp->palette();
      pal.setColor(QPalette::Active, QPalette::Highlight, QColor(r, g, b, 127)); // alpha 127
      qApp->setPalette(pal);
    }
    f.setPointSizeF(f.pointSizeF() * gl->guiScale());
#else
    f.setPointSizeF(f.pointSizeF() * gl->guiScale());
#endif

    a->setFont(f);

    a->setWindowIcon(QIcon(Tpath::img("nootka")));

    auto sound = new Tsound();

    e = new QQmlApplicationEngine;
    e->rootContext()->setContextProperty(QStringLiteral("GLOB"), gl);
    e->rootContext()->setContextProperty(QStringLiteral("Noo"), nooObj);
    e->rootContext()->setContextProperty(QStringLiteral("SOUND"), sound);
    bool  wasFirstRun = gl->isFirstRun;
    TmainHelp* hlp = nullptr; // keep help object live after wizard, Qt deletes it with some delay
    if (gl->isFirstRun) {
      hlp = new TmainHelp();
      e->rootContext()->setContextProperty(QStringLiteral("HELP"), hlp);
      nooObj->setQmlEngine(e);
      e->load(QUrl(QStringLiteral("qrc:/wizard/Wizard.qml")));
      if (firstLoop) {
#if defined (Q_OS_ANDROID)
        qDebug() << "Nootka wizard launch time" << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]";
#else
        QTextStream o(stdout);
        o << "\033[01;35m Nootka wizard launch time: " << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]\033[01;00m\n";
#endif
      }
      exitCode = a->exec();
      e->deleteLater(); // Android crashes without a delayed destroy
      qApp->quit();
      e = new QQmlApplicationEngine;
      e->rootContext()->setContextProperty(QStringLiteral("GLOB"), gl);
      e->rootContext()->setContextProperty(QStringLiteral("Noo"), nooObj);
      e->rootContext()->setContextProperty(QStringLiteral("SOUND"), sound);
      gl->isFirstRun = false;
      gl->config->setValue(QLatin1String("version"), gl->version);
      // TODO: storing current version in settings avoids opening 'about' window next launch
      //       but if there will be another window - delete line above
    }
    nooObj->setQmlEngine(e);
    qmlRegisterType<TnameItem>("Nootka.Main", 1, 0, "TnameItem");
    qmlRegisterType<TmainScoreObject>("Nootka.Main", 1, 0, "TmainScoreObject");
    qmlRegisterType<TdialogLoaderObject>("Nootka.Dialogs", 1, 0, "TdialogObject");

#if defined (Q_OS_ANDROID)
    qmlRegisterType<TmobileMenu>("Nootka", 1, 0, "TmobileMenu");
#endif

    e->load(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

    if (firstLoop) {
#if defined (Q_OS_ANDROID)
     QString androidArg = Tandroid::getRunArgument();
     if (!androidArg.isEmpty())
       nooObj->openFile(androidArg);
#else
      if (argc > 1) {
        if (QString::fromLocal8Bit(argv[argc - 1]).contains(QLatin1String("--nootini")) && !e->rootObjects().isEmpty()) {
            qmlRegisterType<TaudioAnalyzeItem>("Nootka.Main", 1, 0, "TaudioAnalyzeItem");
            QMetaObject::invokeMethod(e->rootObjects().first(), "audioAnalyze");
        } else
            nooObj->openFile(QString::fromLocal8Bit(argv[argc - 1]));
      }
#endif
    }
    if (firstLoop && !wasFirstRun) {
#if defined (Q_OS_ANDROID)
      qDebug() << "NOOTKA LAUNCH TIME" << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]";
#else
      QTextStream o(stdout);
      o << "\033[01;35m Nootka launch time: " << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]\033[01;00m\n";
#endif
    }
    sound->init();

    if (firstLoop && !wasFirstRun) {
      // show some dialog when version was changed (news or other info)
      if (!TdialogLoaderObject::checkVersion(e->rootObjects().first())) {
        // or check updates if no version changed
        if (gl->config->value(QLatin1String("Updates/enableUpdates"), true).toBool())
          TdialogLoaderObject::updateCheckInBackground();
      }
    }

    firstLoop = false;
    exitCode = a->exec();

    if (hlp)
      delete hlp;
    delete e;
    delete sound;
    delete gl;
#if defined (Q_OS_ANDROID)
    if (nooObj->resetConfig()) { // delete config file - new Nootka instance will start with first run wizard
        QFile f(confFile);
        f.remove();
        Tandroid::restartNootka(); // and call Nootka after delay
    }
    nooObj->setResetConfig(false); // do - while loop doesn't work with Android
    qApp->quit(); // HACK: calling QApplication::quick() solves hang on x86 when Qt uses native (usually obsolete) SSL libraries
#endif
  } while (nooObj->resetConfig());

  delete nooObj;
  qInstallMessageHandler(0);

  delete a;
  return exitCode;
}
