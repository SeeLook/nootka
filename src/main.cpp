/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "mainwindow.h"
#include "tglobals.h"

Tglobals *gl;
bool resetConfig;

int main(int argc, char *argv[])
{    
	
	QPointer<QApplication> a = 0;
	QPointer<MainWindow> w = 0;
	int exitCode;
	bool firstTime = true;
	QString confFile;
	resetConfig = false;
	
	do {		
		if (a) delete a;
		if (resetConfig) { // delete config file - new Nootka instance will start with first run wizard
				QFile f(confFile);
				f.remove();
		}
		resetConfig = false;
		a = new QApplication(argc, argv);  
		gl = new Tglobals();
		gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
		confFile = gl->config->fileName();
// loading translations
		QString ll = gl->lang;
		if (ll == "")
				ll = QLocale::system().name();
    QTranslator qtTranslator;
#if defined(Q_OS_LINUX)
    qtTranslator.load("qt_" + ll, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#else
    qtTranslator.load("qt_" + ll, gl->path + "lang");
#endif
    a->installTranslator(&qtTranslator);

    QTranslator nooTranslator;
    nooTranslator.load("nootka_" + ll, gl->path + "lang");
    a->installTranslator(&nooTranslator);
// loading nootka font
    QFontDatabase fd;
// #if defined(Q_OS_WIN32)
    int fid = fd.addApplicationFont(gl->path + "fonts/nootka.ttf");
// #else
//     int fid = fd.addApplicationFont(gl->path + "fonts/nootka.otf");
// #endif
    if (fid == -1) {
        QMessageBox::critical(0, "", QCoreApplication::translate("main", "<center>Can not load a font.<br>Try to install nootka.otf manually.</center>"));
        return 111;
    }
// creating main window
    w = new MainWindow();

#if defined (Q_OS_MAC)
	// to alow opening nootka files by clicking them in MacOs finder
    a->installEventFilter(w);
#endif
    w->show();
    if (firstTime && argc > 1)
        w->openFile(QString::fromLocal8Bit(argv[argc - 1]));
		firstTime = false;
		exitCode = a->exec();
		delete w;
		} while (resetConfig);
		
		return exitCode;
}
