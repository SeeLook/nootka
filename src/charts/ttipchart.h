/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TTIPCHART_H
#define TTIPCHART_H

#include <tgraphicstexttip.h>
#include "tqatype.h"
#include "tclef.h"


class TquestionPoint;
class Tnote;
class TkeySignature;
class TfingerPos;

/** This class represent tip displayed when user hovers cursor 
 * over question point in the chart.
 * It has some useful static methods:
 * wrapPixToHtml
 */
class TtipChart : public TgraphicsTextTip
{

public:
  TtipChart(TquestionPoint *point);
  virtual ~TtipChart();
  
      /** Returns html sting with note pixmap generated according to given params. */
  static QString wrapPixToHtml(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor = 4.0, int strNr = 0);
	
			/** Overloaded method - clef is taken from static  */
  static QString wrapPixToHtml(const Tnote& note, bool defClef, TkeySignature key, qreal factor = 4.0, int strNr = 0);
	
      /** Returns html string with question mark in nootka font. */
  static QString insertQMark();
	
			/** Controls default clef for all tips. Primary is @p Tclef::e_treble_G_8down */
	static Tclef defaultClef;
  
private:
  TquestionPoint *m_point;

};

#endif // TTIPCHART_H
