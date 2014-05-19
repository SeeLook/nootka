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

#include "mainwindow.h"
#include <tinitcorelib.h>
#include <QPointer>
#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QDebug>

#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #include <unistd.h>
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif

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
// #if defined (Q_OS_MAC)
//         QApplication::setStyle(new QPlastiqueStyle);
// #endif
// 		qDebug() << a->style()->objectName();
		gl = new Tglobals();
		gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
		confFile = gl->config->fileName();
		initCoreLibrary(gl);
		prepareTranslations(a);
		if (!loadNootkaFont(a))
			return 111;
// creating main window
    w = new MainWindow();

#if defined (Q_OS_MAC)
	// to allow opening nootka files by clicking them in MacOs finder
    a->installEventFilter(w);
#endif
#if defined (Q_OS_ANDROID)
    w->showFullScreen();
#else
    w->show();
#endif
    if (firstTime && argc > 1)
        w->openFile(QString::fromLocal8Bit(argv[argc - 1]));
		firstTime = false;
		exitCode = a->exec();
		delete w;
	} while (resetConfig);
		
	delete gl;
	SLEEP(10); // delayed exit to avoid crash
	return exitCode;
}
