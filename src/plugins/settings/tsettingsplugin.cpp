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


void TsettingsPlugin::init(const QString& argument, QObject* ob, QWidget* parent, Texam* exam) {
  EsettingsMode mode = e_settings;
  if (argument == "exam")
    mode = e_exam;
  else if (argument == "exercise")
    mode = e_exercise;
  m_settings = new TsettingsDialog(parent, mode);
  m_settings->show();
  connect(m_settings, &TsettingsDialog::finished, this, &TsettingsPlugin::closingSlot);
}


TsettingsPlugin::~TsettingsPlugin() {
  delete m_settings;
}


void TsettingsPlugin::closingSlot(int result) {
  if (result == QDialog::Accepted)
    m_lastWord = "Accepted";
  else
    m_lastWord = "Canceled";
}


