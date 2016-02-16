/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "taboutplugin.h"
#include "taboutnootka.h"
#include "tsupportnootka.h"


void TaboutPlugin::init(const QString& argument, TpluginObject* ob, QWidget* parent, Texam* exam) {
  if (argument == QLatin1String("support")) {
      m_dialog = new TsupportStandalone(parent);
  } else {
      m_dialog = new TaboutNootka(parent);
  }
  m_dialog->exec();
}


TaboutPlugin::~TaboutPlugin()
{
  delete m_dialog;
}

