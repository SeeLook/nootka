/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tlevelplugin.h"
#include "tlevelcreatordlg.h"


void TlevelPlugin::init(const QString& argument, TpluginObject* ob, QWidget* parent, Texam* exam) {
  m_creator = new TlevelCreatorDlg(parent);
  if (!argument.isEmpty())
    m_creator->loadLevelFile(argument);
  m_creator->exec();
  m_lastWord = m_creator->communicate();
  ob->emitMessage(m_lastWord);
}


TlevelPlugin::~TlevelPlugin()
{
  delete m_creator;
}



