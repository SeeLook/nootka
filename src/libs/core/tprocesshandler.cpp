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


#include "tprocesshandler.h"
#include <QProcess>
#include <QTextStream>
#include <QApplication>
#include <QDir>
#include <QDebug>



TprocessHandler::TprocessHandler(const QString& exec, QStringList& args, QObject* object) :
	QObject(object),
	m_exec(exec)
{
		m_exec = QDir::fromNativeSeparators(qApp->applicationDirPath() + "/" + exec);
		m_process = new QProcess;
		m_process->setProcessChannelMode(QProcess::MergedChannels);
		connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processSays()));
		connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(processSays()));
#if defined(Q_OS_WIN32)
		m_exec += ".exe";
#endif
		m_process->start(m_exec, args);
		m_process->waitForFinished(-1);
}


TprocessHandler::~TprocessHandler()
{
	delete m_process;
}

void TprocessHandler::processSays() {
	QTextStream stream(m_process);
  QString out = stream.readLine();
  if (out != "") {
		m_lastWord = out.remove("\"");
// 		qDebug() << "processSays: " << out;
  }
}







