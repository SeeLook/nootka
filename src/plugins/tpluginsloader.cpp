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


#include "tpluginsloader.h"
#include "tplugininterface.h"
#include <QStringList>
#include <QDebug>
#include <QPluginLoader>


TpluginsLoader::TpluginsLoader(QObject* parent) :
  QObject(parent),
  m_plugInterface(0)
{
  m_loader = new QPluginLoader(this);
  m_signalNode = new QObject(this);
}


TpluginsLoader::~TpluginsLoader()
{
  if (m_loader->isLoaded()) {
   if (!m_loader->unload())
     qDebug() << "Cannot unload plugin" << m_loader->fileName() << m_loader->errorString();
  }
}


bool TpluginsLoader::load(TpluginsLoader::Etype pluginType) {
  m_type = pluginType;
  QStringList names;
  names << "Level" << "Settings" << "Analyzer" << "Updater" << "Wizard";
  m_loader->setFileName("Nootka" + names[(int)pluginType] + "Plugin");
  if (!m_loader->load()) {
    qDebug() << m_loader->errorString();
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


QString TpluginsLoader::lastWord() {
  if (m_plugInterface)
    return m_plugInterface->lastWord();
  else
    return "";
}







