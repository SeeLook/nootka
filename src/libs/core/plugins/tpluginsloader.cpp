/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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


#include "tpluginsloader.h"
#include "tplugininterface.h"
#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpluginloader.h>


TpluginsLoader::TpluginsLoader(QObject* parent) :
  QObject(parent),
  m_plugInterface(0),
  m_lastValue(-1)
{
  m_loader = new QPluginLoader(this);
  m_signalNode = new TpluginObject(this);
}


TpluginsLoader::~TpluginsLoader()
{
  if (m_loader->isLoaded()) {
    if (!m_loader->unload())
      qDebug() << "Cannot unload plugin" << m_loader->fileName() << qPrintable(m_loader->errorString());
  }
}


bool TpluginsLoader::load(TpluginsLoader::Etype pluginType) {
  m_type = pluginType;
  QStringList names;
  names << QStringLiteral("Level") << QStringLiteral("Settings") << QStringLiteral("Analyzer")
        << QStringLiteral("Updater") << QStringLiteral("Wizard") << QStringLiteral("About")
        << QStringLiteral("Exam");
  QString f = QStringLiteral("Nootka") + names[(int)pluginType] + QStringLiteral("Plugin");
#if defined (Q_OS_WIN)
  f.prepend(QStringLiteral("lib"));
#endif
  m_loader->setFileName(f);
  if (!m_loader->load()) {
    qDebug() << f << m_loader->fileName() << m_loader->errorString();
    return false;
  }
  return true;
}


bool TpluginsLoader::init(const QString& argument, QWidget* parent, Texam* exam) {
  if (m_loader->isLoaded()) {
    QObject *plugin = m_loader->instance();
    if (plugin) {
      m_plugInterface = qobject_cast<TpluginInterface*>(plugin);
      if (m_plugInterface) {
        connect(m_signalNode, &TpluginObject::message, this, &TpluginsLoader::pluginMessage);
        connect(m_signalNode, &TpluginObject::value, this, &TpluginsLoader::pluginValue);
        m_plugInterface->init(argument, m_signalNode, parent, exam);
        return true;
      }
    } else {
      qDebug() << "Cannot create plugin instance" << (int)m_type;
      return false;
    }
  }
  return false;
}











