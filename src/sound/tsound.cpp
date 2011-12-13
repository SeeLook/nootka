/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#include "tsound.h"
#include "tglobals.h"
#include "taudioout.h"
#include "taudioparams.h"


extern Tglobals *gl;

Tsound::Tsound(QObject* parent) : 
  QObject(parent)
{
  if (gl->A->OUTenabled) {
      player = new TaudioOUT(gl->A, gl->path, this);
      m_playable = player->isPlayable();
  }
  else {
      player = 0;
      m_playable = false;
  }
}

Tsound::~Tsound()
{}

void Tsound::play(Tnote note) {
  if(m_playable) {
    player->play(note.getChromaticNrOfNote());
  }
}

void Tsound::acceptSettings() {
  if (gl->A->OUTenabled) {
    if (!player)
        player = new TaudioOUT(gl->A, gl->path, this);
    else
        player->setAudioOutParams(gl->A);
  } else {
    if (player) {
        delete player;
        player = 0;
        m_playable = false;
    }      
  }
}


//-------------------------------- slots ----------------------------------------------------
void Tsound::playingFinished() {

}
