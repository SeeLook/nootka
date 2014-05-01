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


#ifndef TPROCESSHANDLER_H
#define TPROCESSHANDLER_H

#include <nootkacoreglobal.h>
#include <QStringList>

class QProcess;

/*
 * This class handles given process, 
 * also keeps its last communicate
 */
class NOOTKACORE_EXPORT TprocessHandler : public QObject
{
	Q_OBJECT
	
public:
	TprocessHandler(const QString& exec, QStringList& args, QObject* object = 0);
	~TprocessHandler();
	
			/** Returns the last communicate of the process */
	QString lastWord() { return m_lastWord; }
	
protected slots:
	void processSays();
	
private:
	QProcess					 *m_process;
	QString							m_exec, m_lastWord;
};

#endif // TPROCESSHANDLER_H
