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
#include "tstartexamdlg.h"
#include <QDebug>

extern Tglobals *gl;

TexamExecutor::TexamExecutor()
{

    TstartExamDlg *startDlg = new TstartExamDlg;
    QString actTxt;
    TstartExamDlg::Eactions userAct = startDlg->showDialog(actTxt, m_level);
    if (userAct == TstartExamDlg::e_newLevel) {
        qDebug() << "Level: " << m_level.name;
    } else 
      return;

    createQuestionsList();
}


void TexamExecutor::createQuestionsList() {
  // 1. creating array with guitar strings in order of their height
    char strOrder[6] = { 0,1,2,3,4,5};
    char openStr[6];
    for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune[i+1].getChromaticNrOfNote();

    int i = 4;
    while (i > -1) {
        for (int j=i; j < 5 && openStr[strOrder[j]] < openStr[strOrder[j+1]]; j++) {
            char tmp = strOrder[j];
            strOrder[j] = strOrder[j+1];
            strOrder[j+1] = tmp;
        }
        i--;
    }

  // array done

//    for (int i=0; i<6; i++) qDebug() << i << ": " << (int)openStr[strOrder[i]]
//            << " : " << (int)strOrder[i];

  // 2. searching all frets range string by string
    for(int s = 0; s < 6; s++) {
        if (m_level.usedStrings[strOrder[s]])// check string by strOrder
            for (int f = m_level.loFret; f <= m_level.hiFret; f++) {
            Tnote n = Tnote(gl->Gtune[strOrder[s]+1].getChromaticNrOfNote() + f);
            if (n.getChromaticNrOfNote() >= m_level.loNote.getChromaticNrOfNote() &&
                n.getChromaticNrOfNote() <= m_level.hiNote.getChromaticNrOfNote()) {
                bool hope = true; // we stil have hope that note is for exam
                if (m_level.onlyLowPos) {
                    if (s > 0) {
                       // we check when level has only lowest positions
                       // is it realy lowest pos
                       // when is 0 - it is the highest sting
                       char diff = openStr[strOrder[s-1]] - openStr[s];
                       if( (f-diff) >= m_level.loFret && (f-diff) <= m_level.hiFret)
                           hope = false;
                       else
                           hope = true;
                    }
                }
                if (hope && m_level.useKeySign && m_level.onlyCurrKey) {
                  hope = false;
                  if (m_level.isSingleKey) {
                    if(m_level.loKey.inKey(n).note != 0)
                        hope = true;
                    } else {
                        for (int k = m_level.loKey.value(); k <= m_level.hiKey.value(); k++) {
                          if (TkeySignature::inKey(TkeySignature(k), n).note != 0) {
                            hope = true;
                            break;
                          }
                        }
                    }
                }
                if (hope) {
                    TQAunit::TQAgroup g;
                    g.note = n; g.pos = TfingerPos(strOrder[s]+1, f);
                    m_questList << g;
                }
            }
        }
    }

    for (int i = 0; i < m_questList.size(); i++)
        qDebug() << i << (int)m_questList[i].pos.str() << "f"
                << (int)m_questList[i].pos.fret() << " note: "
                << QString::fromStdString(m_questList[i].note.getName());
}



