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


#ifndef TFINISHTIP_H
#define TFINISHTIP_H
#include <tgraphicstexttip.h>

class Texam;

QString finishExamText(Texam *exam);


/** QGraphicsTextTip with exam summary like piece of paper. */
class TfinishTip : public TgraphicsTextTip
{
    Q_OBJECT
    
public:
    TfinishTip(Texam *exam);
    virtual ~TfinishTip() {}
    
private:
    Texam *m_exam;
    
};

#endif // TFINISHTIP_H
