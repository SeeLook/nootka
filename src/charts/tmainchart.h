/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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

#ifndef TMAINCHART_H
#define TMAINCHART_H

#include "tchart.h"


class Texam;


class TmainChart : public Tchart
{
  Q_OBJECT
  
public:
  
  TmainChart(Texam *exam, QWidget* parent = 0);
  virtual ~TmainChart();
  
  void setAnalyse(EanswersOrder order);
  
  void doAnalyseByNumber();
  void doAnalyseByNote();
  
  
  
private:
  Texam *m_exam;
  TmainLine *m_mainLine;
  EanswersOrder m_analysType;
};

#endif // TMAINCHART_H
