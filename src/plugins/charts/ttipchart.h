/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TTIPCHART_H
#define TTIPCHART_H

#include <graphics/tgraphicstexttip.h>
#include <music/tclef.h>


class TquestionPoint;


/**
 * This class represent tip displayed when user hovers cursor 
 * over question point in the chart.
 */
class TtipChart : public TgraphicsTextTip
{

public:
  TtipChart(TquestionPoint *point);
	
			/** Controls default clef for all tips. Primary is @p Tclef::e_treble_G_8down */
	static Tclef defaultClef;
  
protected:
  TquestionPoint* qa() { return m_point; } /** returns pointer to question/answer which this point describing. */
  
private:
  TquestionPoint *m_point;

};

#endif // TTIPCHART_H
