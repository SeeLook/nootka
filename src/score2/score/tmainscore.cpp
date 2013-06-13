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

#include "tmainscore.h"
#include "tscorestaff.h"
#include "ttune.h"

// TODO: change this to gl when merged
Ttune glTune;

TmainScore::TmainScore(int noteNumber, QWidget* parent) :
	TsimpleScore(noteNumber, parent)
{
	glTune = Ttune::tunes[3];
	
	
	connect(this, SIGNAL(pianoStaffSwitched()), this, SLOT(onPianoSwitch()));
}

TmainScore::~TmainScore()
{
}


//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void TmainScore::setScordature() {
	staff()->setScordature(glTune);
	resizeEvent(0);
}


//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmainScore::onPianoSwitch() {
	if (glTune != Ttune::stdTune)
			setScordature();
}







