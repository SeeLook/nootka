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

#ifndef NCHART_H
#define NCHART_H

#include <tchart.h>

class NoteData;
class AnalysisData;
class TpitchFinder;

/**
 * Nootini chart
 */
class Nchart : public Tchart
{

public:
  Nchart(QWidget* parent = 0);

  void setPitchFinder(TpitchFinder* pf); /** Pitch finder processing audio data */
  void setXnumber(int xN); /** Number of X values on the chart */

protected:
  void chunkSlot(AnalysisData* ad, NoteData* nd);

private:
  TpitchFinder                *m_pitchF;
  int                          m_chunkNr;
};

#endif // NCHART_H
