/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include <QtGui>
#include "mainwindow.h"
#include "tglobals.h"

Tglobals *gl;

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN32)
    QApplication::setStyle("plastique");  
#endif
		
  QApplication a(argc, argv);
	gl = new Tglobals();
	gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
	
	qDebug() << gl->path;
    
	QString ll = gl->lang;
	if (ll == "")
		ll = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + ll, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);


    QTranslator nooTranslator;
    nooTranslator.load("nootka_" + ll, gl->path + "lang");
    a.installTranslator(&nooTranslator);
	
    MainWindow w;

    QFontDatabase fd;
    if (fd.addApplicationFont(gl->path + "fonts/nootka.otf") == -1) {
        QMessageBox::critical(0, "", QCoreApplication::translate("main", "<center>Can not load a font.<br>Try to install nootka.otf manually.</center>"));
        return 111;
    }

    w.show();
    if (argc > 1)
        w.openFile(QString::fromLocal8Bit(argv[argc-1]));
    return a.exec();
}
