/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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


#include "tabstractsniffer.h"
#include "taudioparams.h"


TabstractSniffer::TabstractSniffer(TaudioParams* params, QObject *parent) :
  QObject(parent),
  m_stoppedByUser(params->stoppedByUser),
  m_state(e_stopped)
{
  p_audioParams = params;
}


void TabstractSniffer::setStoppedByUser(bool userStop) {
  m_stoppedByUser = userStop;
  p_audioParams->stoppedByUser = userStop;
}


void TabstractSniffer::setAmbitus(const Tnote& lowestNote, const Tnote& highestNote) {
  m_loNote = Tnote(lowestNote.chromatic() + p_audioParams->transposition);
  m_hiNote = Tnote(highestNote.chromatic() + p_audioParams->transposition);
}
