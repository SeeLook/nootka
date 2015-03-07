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

#ifndef H_ANALYZEDATA
#define H_ANALYZEDATA

#include <QtCore/QtGlobal>

/**
 * Structure of values detecting by @class TpitchFinder
 * to process them in analysis chart
 */
class NanalyzeData
{
public:
  NanalyzeData(int _index, float _pitch = 0, double _averPitch = 0, double _vol = 0, double _dur = 0) :
    index(_index),
    pitch(_pitch), basePitch(qRound(_pitch)),
    avPitch(_averPitch), vol(_vol), dur(_dur)
  {}

  int index; /** Note index in entire channel */
  float pitch; /** pitch in chunk */
  int basePitch; /** Midi value (rounded to integer) of a pitch */
  double avPitch; /** Average pitch  */
  double vol; /** volume */
  double dur; /** Note duration */
  float signalStrenght; /** Max amplitude of PCM data in chunk  */
};


#endif // H_ANALYZEDATA
