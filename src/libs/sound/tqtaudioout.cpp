/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "tqtaudioout.h"
#include <taudioparams.h>
#include <QDebug>
#include <QTimer>



TaudioOUT::TaudioOUT(TaudioParams* _params, QObject* parent) :
  TabstractPlayer(parent),
  m_audioParams(_params)
{
  setType(e_audio);
  setAudioOutParams();
//   connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedSlot);
}


TaudioOUT::~TaudioOUT() 
{
  
}


void TaudioOUT::setAudioOutParams() {
}



bool TaudioOUT::play(int noteNr) {
  if (!playable /*|| audioParams()->forwardInput*/) // if forwarding is enabled play() makes no sense
      return false;
  
  
//  noteNr = noteNr + int(audioParams()->a440diff);
	
  doEmit = true;
}


void TaudioOUT::playingFinishedSlot() {

}


void TaudioOUT::stop() {
	
}



