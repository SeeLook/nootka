/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "texamexecutor.h"
#include "tglobals.h"
//#include <QDebug>

extern Tglobals *gl;

TexamExecutor::TexamExecutor(TexamLevel level)
{


    createQuestionsList();
}

void TexamExecutor::createQuestionsList() {
    char strOrder[6] = { 0,1,2,3,4,5};
    char openStr[6];
    for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune[i+1].getChromaticNrOfNote();

    int i = 1;
    while (i < 6) {
        for (int j=i; j > 0 && openStr[strOrder[j-1]] > openStr[strOrder[j]]; j--) {
            char tmp = strOrder[j];
            strOrder[j] = strOrder[j-1];
            strOrder[j-1] = tmp;
        }
        i++;
    }

    char ord[6];
    for (int i=0; i<6; i++) {
       ord[i] = strOrder[5-i];
    }
}
