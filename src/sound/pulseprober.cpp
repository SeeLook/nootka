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


#include "pulseprober.h"
#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include <complex>



QString searchDir(QString dirName, QString keyWord) {
    QDir dir(dirName);
    QFileInfoList dirContent = dir.entryInfoList();
    for (int i=0; i < dirContent.size(); i++) {
      QFileInfo fileInfo = dirContent.at(i);
      if (fileInfo.fileName().contains(keyWord))
        return fileInfo.absoluteFilePath();
    }
    return "";
}

bool hasPulsePlugin() {
    QStringList pluginPaths = qApp->libraryPaths();
    for (int i=0; i < pluginPaths.size(); i++) {
      if (pluginPaths[i].contains("plugin")) {
        QDir plugDir(pluginPaths[i]);
        QFileInfoList dirList = plugDir.entryInfoList();
        for (int j = 0; j < dirList.size(); j++) {
            QString audioDir = searchDir(dirList.at(j).absoluteFilePath(), "audio");
            QString pulsePlug = searchDir(audioDir, "pulse");
            if (pulsePlug != "") {
              qDebug() << "Pulseaudio Qt plugin found";
              return true;
            }
        }
      }
    }
    return false;
}


bool checkForPulse() {
    if (hasPulsePlugin()) {
      qDebug() << "QT complied with pulse.\nChecking is pulseaudio available...";
//       QProcess *ps = new QProcess();
//       QStringList args;
//       args << "pulse";
//       ps->start("pgrep", args);
//       ps->waitForFinished();
//       QString exitText(ps->readAllStandardOutput());
//       delete ps;
      QFileInfo pulseBin("/usr/bin/pulseaudio");
      if (pulseBin.exists())  
        return true;
      else {
        qDebug() << "PULSEAUDIO IS NOT RUNNING !!!!!!";
        return false;
      }
    }
    return true;
}

//##########################################################################################
//#######################    TpulseWarring  ################################################
//##########################################################################################

TpulseWarring::TpulseWarring(QWidget* parent):
    QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
    setText("<b><span style=\"font-size: 20px; color: #ff0000;\">PulseAudio is not installed or doesn't work.<br>Only MIDI output can be available</span></b>");
    setWordWrap(true);
}




