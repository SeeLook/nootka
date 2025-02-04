/***************************************************************************
 *   Copyright (C) 2011-2025 by Tomasz Bojczuk                             *
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

#include "dialogs/tdialogloaderobject.h"
#include "help/tmainhelp.h"
#include "main/tgotit.h"
#include "main/tmainscoreobject.h"
#include "main/tnameitem.h"
#include <tglobals.h>
#include <tinitcorelib.h>
#include <tmtr.h>
#include <tnootkaqml.h>
#include <tpath.h>
#include <tsound.h>

#if defined(Q_OS_ANDROID)
#include "mobile/tmobilemenu.h"
#include <Android/tandroid.h>
#else
#include "nootini/taudioanalyzeitem.h"
#include <QtCore/qcommandlineparser.h>
#endif

#include <QtCore/qdatetime.h>
#include <QtCore/qdir.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qfile.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qpointer.h>
#include <QtCore/qsettings.h>
#include <QtCore/qtranslator.h>
#include <QtGui/qicon.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtWidgets/qapplication.h>

#include <QtCore/qdebug.h>

static QString logFile;

/**
 * It allows to grab all debug messages into nootka-log.txt file
 */
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Q_UNUSED(type)

#if defined(Q_OS_ANDROID)
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
#if defined(Q_OS_WIN)
    // It works under Win
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // It mutes QML warnings about connections syntax introduced in Qt 5.15
    // TODO when Qt version requirements will rise to 5.15 or above, change syntax and remove that
    QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.connections=false"));

#if defined(Q_OS_ANDROID)
    qputenv("QT_ANDROID_VOLUME_KEYS", "1"); // Handle volume keys by Qt, lock native Android behavior

    // log to any writable storage
    if (Tandroid::hasWriteAccess()) {
        logFile = Tandroid::getExternalPath() + QStringLiteral("/nootka-log.txt");
        Tandroid::askForWriteAcces();
        if (QFile::exists(logFile))
            QFile::remove(logFile);
        qInstallMessageHandler(myMessageOutput);
        qDebug() << "==== NOOTKA LOG =======\n" << QDateTime::currentDateTime().toString();
    }
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
#if !defined(Q_OS_ANDROID)
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
#if defined(Q_OS_ANDROID)
        f.setPixelSize(nooObj->factor());
        auto pal = qApp->palette();
        pal.setColor(QPalette::Active, QPalette::Highlight, QColor(0, 160, 160)); // Teal color of highlight for Android
        pal.setColor(QPalette::Active, QPalette::Shadow, QColor(120, 120, 120)); // Dark gray for shadow
        pal.setColor(QPalette::Active, QPalette::Window, QColor(250, 250, 250)); // Almost white for windows
        pal.setColor(QPalette::Active, QPalette::Base, QColor(255, 255, 255)); // White base
        pal.setColor(QPalette::Active, QPalette::Button, QColor(240, 240, 240)); // Very light gray for button
        pal.setColor(QPalette::Active, QPalette::AlternateBase, QColor(245, 255, 255)); // Very light teal for alternate base
        qApp->setPalette(pal);
#elif defined(Q_OS_WIN)
        QSettings accent(QStringLiteral("HKEY_USERS\\.DEFAULT\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent"), QSettings::NativeFormat);
        if (accent.contains(QLatin1String("StartColorMenu"))) {
            int color = accent.value(QLatin1String("StartColorMenu")).toInt();
            int r = color & 0xff;
            int g = (color >> 8) & 0xff;
            int b = (color >> 16) & 0xff;
            auto pal = qApp->palette();
            QColor c(r, g, b);
            pal.setColor(QPalette::Active, QPalette::Highlight, c.lighter(150));
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

        qmlRegisterSingletonType<Tglobals>("Nootka", 1, 0, "GLOB",
                                          [&](QQmlEngine*, QJSEngine*)->QObject* {
                                              return gl;
                                          });

        qmlRegisterSingletonType<TnootkaQML>("Nootka", 1, 0, "NOO",
                                           [&](QQmlEngine*, QJSEngine*)->QObject* {
                                               return nooObj;
                                           });

        qmlRegisterSingletonType<Tsound>("Nootka", 1, 0, "SOUND",
                                             [&](QQmlEngine*, QJSEngine*)->QObject* {
                                                 return sound;
                                             });

        bool wasFirstRun = gl->isFirstRun;
        TmainHelp *hlp = nullptr; // keep help object live after wizard, Qt deletes it with some delay
        if (gl->isFirstRun) {
            hlp = new TmainHelp();
            e->rootContext()->setContextProperty(QStringLiteral("HELP"), hlp);
            nooObj->setQmlEngine(e);
            e->load(QUrl(QStringLiteral("qrc:/wizard/Wizard.qml")));
            if (firstLoop) {
#if defined(Q_OS_ANDROID)
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

            qmlRegisterSingletonType<Tglobals>("Nootka", 1, 0, "GLOB",
                                               [&](QQmlEngine*, QJSEngine*)->QObject* {
                                                   return gl;
                                               });

            qmlRegisterSingletonType<TnootkaQML>("Nootka", 1, 0, "NOO",
                                                 [&](QQmlEngine*, QJSEngine*)->QObject* {
                                                     return nooObj;
                                                 });

            qmlRegisterSingletonType<Tsound>("Nootka", 1, 0, "SOUND",
                                                 [&](QQmlEngine*, QJSEngine*)->QObject* {
                                                     return sound;
                                                 });

            gl->isFirstRun = false;
            gl->config->setValue(QLatin1String("version"), gl->version);
            // TODO: storing current version in settings avoids opening 'about' window next launch
            //       but if there will be another window - delete line above
        }
        nooObj->setQmlEngine(e);
        qmlRegisterType<TgotIt>("Nootka.Main", 1, 0, "TgotIt");
        qmlRegisterType<TnameItem>("Nootka.Main", 1, 0, "TnameItem");
        qmlRegisterType<TmainScoreObject>("Nootka.Main", 1, 0, "TmainScoreObject");
        qmlRegisterType<TdialogLoaderObject>("Nootka.Dialogs", 1, 0, "TdialogObject");

#if defined(Q_OS_ANDROID)
        qmlRegisterType<TmobileMenu>("Nootka", 1, 0, "TmobileMenu");
#endif

        e->load(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

#if !defined(Q_OS_ANDROID)
        if (e->rootObjects().isEmpty()) {
            QTextStream o(stdout);
            o << "\033[0;31m Something went wrong and Nootka was not able to launch.\033[01;00m\n";
            return 121;
        }
#endif

        if (firstLoop && !wasFirstRun) {
#if defined(Q_OS_ANDROID)
            qDebug() << "NOOTKA LAUNCH TIME" << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]";
#else
            QTextStream o(stdout);
            o << "\033[01;35m Nootka launch time: " << startElapsed.nsecsElapsed() / 1000000.0 << " [ms]\033[01;00m\n";
#endif
        }

        if (firstLoop) {
#if defined(Q_OS_ANDROID)
            QString androidArg = Tandroid::getRunArgument();
            if (!androidArg.isEmpty())
                nooObj->openFile(androidArg);
#else
            if (argc > 1) {
                QCommandLineParser cmd;
                auto helpOpt = cmd.addHelpOption();
                QCommandLineOption dumpOpt(QStringList() << QStringLiteral("dump-dir") << QStringLiteral("d"),
                                           QStringLiteral("Full path to directory where audio data used to pitch detection will be dumped.\n"
                                                          "Dumped files can be further analyzed with 'nootini' option.\n"),
                                           QStringLiteral("existing dir"));
                cmd.addOption(dumpOpt);
                QCommandLineOption nootiniOpt(QStringList() << QStringLiteral("nootini") << QStringLiteral("n"),
                                              QStringLiteral("Launch Nootka in audio analyze mode. Nootini: (Nootka + Tartini)\n"));
                cmd.addOption(nootiniOpt);
                QCommandLineOption audioFileOpt(QStringList() << QStringLiteral("audio-file") << QStringLiteral("a"),
                                                QStringLiteral("Audio file to analyze. Only raw files dumped by Nootka are supported.\n"),
                                                QStringLiteral("wav or raw audio"));
                cmd.addOption(audioFileOpt);
                QCommandLineOption tempoOpt(QStringList() << QStringLiteral("tempo") << QStringLiteral("t"),
                                            QStringLiteral("Tempo of given audio file.\n"),
                                            QStringLiteral("bpm"));
                cmd.addOption(tempoOpt);
                QCommandLineOption instrOpt(QStringList() << QStringLiteral("instrument") << QStringLiteral("i"),
                                            QStringLiteral("Instrument to use. See Tinstrument class (tinstrument.h)\n"),
                                            QStringLiteral("number"));
                cmd.addOption(instrOpt);
                QCommandLineOption quantOpt(QStringList() << QStringLiteral("quantization") << QStringLiteral("q"),
                                            QStringLiteral("Quantization (round to): sixteenths (6) or eights (12)\n"),
                                            QStringLiteral("6 or 12"));
                cmd.addOption(quantOpt);
                QCommandLineOption minVolOpt(QStringList() << QStringLiteral("min-volume") << QStringLiteral("m"),
                                             QStringLiteral("Minimal note volume to be pitch detected in percents.\n"),
                                             QStringLiteral("20-80"));
                cmd.addOption(minVolOpt);
                QCommandLineOption clefOpt(QStringList() << QStringLiteral("clef") << QStringLiteral("c"),
                                           QStringLiteral("Clef for score. See Tclef class (tclef.h)\n"),
                                           QStringLiteral("number (power of 2)"));
                cmd.addOption(clefOpt);

                /** Option below is handled internally by @p TnootkaQML. */
                cmd.addOptions({{QStringLiteral("no-version"), QStringLiteral("Do not display app version.\n")}});

                cmd.parse(a->arguments());
                if (cmd.isSet(helpOpt))
                    cmd.showHelp();

                if (cmd.isSet(dumpOpt))
                    SOUND->changeDumpPath(cmd.value(dumpOpt));
                else if (cmd.isSet(nootiniOpt)) {
                    qmlRegisterType<TaudioAnalyzeItem>("Nootka", 1, 0, "TaudioAnalyzeItem");
                    QMetaObject::invokeMethod(e->rootObjects().first(), "audioAnalyze");
                    if (cmd.isSet(instrOpt)) {
                        int instr = cmd.value(instrOpt).toInt();
                        GLOB->setTransposition(Tinstrument(static_cast<Tinstrument::Etype>(instr)).transposition());
                        GLOB->setInstrument(instr);
                    }
                    if (cmd.isSet(quantOpt))
                        SOUND->setQuantization(cmd.value(quantOpt).toInt());
                    if (cmd.isSet(tempoOpt))
                        SOUND->setTempo(cmd.value(tempoOpt).toInt());
                    if (cmd.isSet(minVolOpt))
                        GLOB->setMinVolume(static_cast<qreal>(qBound(20, cmd.value(minVolOpt).toInt(), 80)) / 100.0);
                    if (cmd.isSet(audioFileOpt))
                        TaudioAnalyzeItem::processAudioFile(cmd.value(audioFileOpt));
                    if (cmd.isSet(clefOpt))
                        GLOB->setClefType(cmd.value(clefOpt).toInt());
                } else
                    nooObj->openFile(QString::fromLocal8Bit(argv[argc - 1]));
            }
#endif
        }

        sound->init();

        if (firstLoop && !wasFirstRun && !e->rootObjects().isEmpty()) {
            // show dialog with changes when version was changed
            if (!TdialogLoaderObject::checkVersion(e->rootObjects().first())) {
                // or check to dispay support dialog
                if (!TdialogLoaderObject::checkForSupport(e->rootObjects().first())) {
                    // or check for updates if any of above
                    if (gl->config->value(QLatin1String("Updates/enableUpdates"), true).toBool())
                        TdialogLoaderObject::updateCheckInBackground();
                }
            }
        }

        firstLoop = false;
        exitCode = a->exec();

        if (hlp)
            delete hlp;
        delete e;
        // delete sound; // singleton -  deleted by engine
        // delete gl; // singleton -  deleted by engine
#if defined(Q_OS_ANDROID)
        if (nooObj->resetConfig()) { // delete config file - new Nootka instance will start with first run wizard
            QFile f(confFile);
            f.remove();
            Tandroid::restartNootka(); // and call Nootka after delay
        }
        nooObj->setResetConfig(false); // do - while loop doesn't work with Android
        qApp->quit(); // HACK: calling QApplication::quick() solves hang on x86 when Qt uses native (usually obsolete) SSL libraries
#endif
    } while (nooObj->resetConfig());

    // delete nooObj; // singleton -  deleted by engine
    qInstallMessageHandler(0);

    delete a;
    return exitCode;
}
