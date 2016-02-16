/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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


#include "texamplugin.h"
#include "texamexecutor.h"
#include <plugins/tpluginsloader.h>
#include <exam/tlevel.h>
#include <level/tlevelselector.h>
#include <QtWidgets/qwidget.h>
#include <QtCore/qdebug.h>


void TexamPlugin::init(const QString& argument, TpluginObject* ob, QWidget* parent, Texam* exam) {
  m_parentWidget = parent;
  m_sender = ob;
  m_level = nullptr;
  QString executorArgs;
  if (!argument.isEmpty()) {
    if (argument.left(5) == QLatin1String("file:")) {
        executorArgs = argument.mid(5);
    } else if (argument.contains(QLatin1String("level:"))) {
        QString levelString = argument;
        levelString.remove(QLatin1String("level:"));
        if (levelString.contains(QLatin1String(":exercise"))) {
            levelString.remove(QLatin1String(":exercise"));
            executorArgs = QLatin1String("exercise");
        }
        TlevelSelector ls;
        ls.selectLevel(levelString.toInt());
        m_level = new Tlevel;
        *m_level = ls.getSelectedLevel();
    }
  }
  m_executor = new TexamExecutor(this);
  connect(m_executor, &TexamExecutor::examMessage, [=](int v){ m_sender->emitValue(v); });
  connect(m_sender, &TpluginObject::backValue, this, &TexamPlugin::backValueSlot);
  m_executor->init(executorArgs, m_level);
}


TexamPlugin::~TexamPlugin()
{
  if (m_level)
    delete m_level;
}


void TexamPlugin::backValueSlot(int v) {
  switch ((Torders::Eexam)v) {
    case Torders::e_examSettings: {
        QString args;
        if (m_executor->isExercise())
            args = QStringLiteral("exercise");
        else
            args = QStringLiteral("exam");
        m_executor->prepareToSettings();
        TpluginsLoader *settingsPlugin = new TpluginsLoader();
        if (settingsPlugin->load(TpluginsLoader::e_settings))
            settingsPlugin->init(args, m_parentWidget);
        Torders::Esettings lastValue = (Torders::Esettings)settingsPlugin->lastValue();
        settingsPlugin->deleteLater();
        if (lastValue == Torders::e_settingsAccept)
            m_executor->settingsAccepted();
    }
        break;

    case Torders::e_examClose: {
        if (m_executor->closeNootka())
          m_sender->emitValue(Torders::e_examAllowClose);
    }
        break;

    default:
        break;
  }
}


