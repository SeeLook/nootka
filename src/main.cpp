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

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFontDatabase fd;
    QStringList fl = QStringList(fd.families(QFontDatabase::Any));
    if (! fl.contains("Emmentaler")) {
        int nr[8] =  {11,13,14,16,18,20,23,26};
        for (int i=0; i<8; i++)
            fd.addApplicationFont(QString(":/fonts/emmentaler-%1.otf").arg(nr[i]));
//        QMessageBox msg;
//        msg.setText(QT_TR_NOOP("<em>Emmentaler</em> fonts were not found. Please, install them manually first.<br>You can find them either in installatiin package or under address"));
//        msg.exec();
//        return 111;
    }


    w.show();

    return a.exec();
}
