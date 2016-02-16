/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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


#include "tsettingsplugin.h"
#include "tsettingsdialog.h"


void TsettingsPlugin::init(const QString& argument, TpluginObject* ob, QWidget* parent, Texam* exam) {
  EsettingsMode mode = e_settings;
  if (argument == QLatin1String("exam"))
      mode = e_exam;
  else if (argument == QLatin1String("exercise"))
      mode = e_exercise;
  m_settings = new TsettingsDialog(parent, mode);
  Torders::Esettings lastValue;
  if (m_settings->exec() == QDialog::Accepted)
      lastValue = Torders::e_settingsAccept;
  else
      lastValue = Torders::e_settingsCancel;
  if (m_settings->resetToDefaults())
      lastValue = Torders::e_settingsReset;
  ob->emitValue(lastValue);
}


TsettingsPlugin::~TsettingsPlugin() {
  delete m_settings;
}



