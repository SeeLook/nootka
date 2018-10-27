/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#ifndef TSTATISTICSTIP_H
#define TSTATISTICSTIP_H

#include <graphics/tgraphicstexttip.h>

class TgroupedQAunit;


/** This class represents a tip chart with statistics of grouped questions.*/
class TstatisticsTip : public TgraphicsTextTip
{
  
public:
      
        /** Determines how many data are shown in tip
        * e_full - (default) average time, questions number and all kinds of mistakes.
        * e_simple - average time only (It is necessary to give description in constructor). 
        * e_mistakes - sort by mistakes, skips effectivenes and questions number
        */
    enum Ekind { e_full, e_simple, e_mistakes };
  
    TstatisticsTip(TgroupedQAunit *qaGroup, Ekind kind = e_full, QString desc = "");
    virtual ~TstatisticsTip();
        /** Returns html-formated full information aobout grouped questions. */
    static QString getTipText(TgroupedQAunit *qaGroup);
        /** Returns:
        * Average reaction time
        * of something
        *      0.0 s
        */
    static QString getAverTimeStat(TgroupedQAunit *qaGroup, QString ofSomething);
    
private:
    TgroupedQAunit *m_qaGroup;
    static Ekind m_kind;
    
};

#endif // TSTATISTICSTIP_H
