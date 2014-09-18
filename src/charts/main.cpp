/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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


#include "tanalysdialog.h"
#include <tinitcorelib.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <QApplication>
#include <QTranslator>
#include <QDebug>

Tglobals *gl;

int main(int argc, char *argv[])
{    	
		QTranslator qtTranslator;
		QTranslator qtbaseTranslator;
		QTranslator nooTranslator;
		QApplication a(argc, argv);
// #if defined (Q_OS_MAC)
// 		QApplication::setStyle(new QPlastiqueStyle);
// #endif
		
		gl = new Tglobals();
		gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
		initCoreLibrary(gl);
		prepareTranslations(&a, qtTranslator, qtbaseTranslator, nooTranslator);
		if (!loadNootkaFont(&a))
			return 111;

		TanalysDialog *analyzer = 0;
		Tlevel *level = 0;
		Texam *exam = 0;
		QStringList args = a.arguments();
		args.removeFirst();
		if (args.size()) {
			level = new Tlevel();
			exam = new Texam(level, "");
			Texam::EerrorType err = exam->loadFromFile(args.first());
			if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
				analyzer = new TanalysDialog(exam);
				if (args.size() >= 2 && args[1] == "exercise")
					analyzer->setWindowTitle(analyzer->analyseExerciseWinTitle());
			} else 
				qDebug() << "Can not read from exam file" << args.first() << (int)err;
		} else {
				analyzer = new TanalysDialog();
			
		}
		int retVal = -1;
		if (analyzer) {
			analyzer->show();
			retVal = a.exec();
		}
		
		delete exam;
		delete level;
		return retVal;		
}





