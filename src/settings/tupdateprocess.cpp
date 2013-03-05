/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tupdateprocess.h"
#include <QProcess>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTimer>

TupdateProcess::TupdateProcess(bool respectRules, QObject* parent) : 
  QObject(parent),
  m_respectRules(respectRules)
{
  m_process = new QProcess(this);
  m_exec = qApp->applicationDirPath() + "/" + "nootka-updater";
  QFileInfo fi(m_exec);
  if (fi.exists()) {
    m_isPossible = true;
  } else
    m_isPossible = false;
  connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processSays()));
//   connect(m_process, SIGNAL(readChannelFinished()), this, SLOT(processSays()));
}



TupdateProcess::~TupdateProcess()
{
  delete m_process;
}

void TupdateProcess::start() {
  if (m_isPossible) {
    m_timer = new QTimer(this);
    m_timer->start(7000); // 7 sec for check updates
    connect(m_timer, SIGNAL(timeout()), this, SLOT(processTimeOut()));
    QStringList args;
    if (m_respectRules)
      args << "1";
    m_process->start(m_exec, args);
  }
}


void TupdateProcess::processSays() {
  QString out = QString(m_process->readAllStandardOutput());
  if (out != "") {
    if (out == "success\n" || out.contains("need"))
      m_timer->stop();
    else {
      qDebug() << "processSays: " << out;
      emit updateOutput(QString(out));
    }
  }
}



void TupdateProcess::processTimeOut() {
//   if (m_process->state() == QProcess::NotRunning)
//     return;
//   else {
    m_timer->stop();
    qDebug() << "processSays: time expired";
    emit updateOutput("time expired");
    m_process->kill();
//   }
}
