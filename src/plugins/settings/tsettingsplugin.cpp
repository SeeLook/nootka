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


#include "tsettingsplugin.h"
#include "tsettingsdialog.h"
#include <QTimer>

void TsettingsPlugin::init(const QString& argument, TpluginObject* ob, QWidget* parent, Texam* exam) {
  EsettingsMode mode = e_settings;
  if (argument == "exam")
    mode = e_exam;
  else if (argument == "exercise")
    mode = e_exercise;
  m_settings = new TsettingsDialog(parent, mode);
  if (m_settings->exec() == QDialog::Accepted)
    m_lastWord = "Accepted";
  else
    m_lastWord = "Canceled";
  if (m_settings->resetToDefaults())
    m_lastWord = "Reset";
  ob->emitMessage(m_lastWord);
}


TsettingsPlugin::~TsettingsPlugin() {
  delete m_settings;
}



