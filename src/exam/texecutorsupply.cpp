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

#include "texecutorsupply.h"
#include "tglobals.h"
#include "texamlevel.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

extern Tglobals *gl;

TexecutorSupply::TexecutorSupply(TexamLevel* level, QObject* parent) :
  QObject(parent),
  m_level(level),
  m_prevAccid(Tnote::e_Natural),
  m_isSolfege(false),
  m_dblAccidsCntr(0),
  m_eisCesCntr(0),
  m_wasFinished(false)
{
  calcQAPossibleCount();
}

//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################

void TexecutorSupply::examFinished() {
    QMessageBox::information(0, "", "<br><center>" + tr("CONGRATULATIONS!!!<br>You have passed the exam.<br><br>About prizes speak with parents or another sponsors :-)<br>You can still play with it and improve effectivenes.") + "</center><br>");
    setFinished();
}


void TexecutorSupply::createQuestionsList(QList<TQAunit::TQAgroup> &list) {
    char openStr[6];
      for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune()[i+1].getChromaticNrOfNote();
      
      /** FIXING MISTAKE RELATED WITH A NEW VALIDATIN WAY DURING SAVING NEW LEVEL 
       * When there in no guitar in a level,
       * add to question list only the lowest position sounds. */
    if (!m_level->canBeGuitar()) {  // adjust frets' range 
      m_level->onlyLowPos = true;
    }

// searching all frets in range, string by string
    for(int s = 0; s < 6; s++) {
        if (m_level->usedStrings[gl->strOrder(s)])// check string by strOrder
            for (int f = m_level->loFret; f <= m_level->hiFret; f++) {
                Tnote n = Tnote(gl->Gtune()[gl->strOrder(s)+1].getChromaticNrOfNote() + f);
            if (n.getChromaticNrOfNote() >= m_level->loNote.getChromaticNrOfNote() &&
                n.getChromaticNrOfNote() <= m_level->hiNote.getChromaticNrOfNote()) {
                bool hope = true; // we stil have hope that note is for an exam
                if (m_level->onlyLowPos) {
                    if (s > 0) {
                       // we have to check when note is on the lowest positions
                       // is it realy lowest pos
                       // when strOrder[s] is 0 - it is the highest sting
                        char diff = openStr[gl->strOrder(s-1)] - openStr[gl->strOrder(s)];
                       if( (f-diff) >= m_level->loFret && (f-diff) <= m_level->hiFret) {
                           hope = false; //There is the same note on highest string
                       }
                       else {
                           hope = true;
                       }
                    }
                }
                if (hope && m_level->useKeySign && m_level->onlyCurrKey) {
                  hope = false;
                  if (m_level->isSingleKey) {
                    if(m_level->loKey.inKey(n).note != 0)
                        hope = true;
                    } else {
                        for (int k = m_level->loKey.value(); k <= m_level->hiKey.value(); k++) {
                          if (TkeySignature::inKey(TkeySignature(k), n).note != 0) {
                            hope = true;
                            break;
                          }
                        }
                    }
                }
                if (hope) {
                    if (n.acidental && (!m_level->withFlats && !m_level->withSharps))
                        continue;
                    else {
                        TQAunit::TQAgroup g;
                        g.note = n; g.pos = TfingerPos(gl->strOrder(s)+1, f);
                        list << g;
                    }
                }
            }
        }
    }

//    for (int i = 0; i < list.size(); i++)
//        qDebug() << i << (int)list[i].pos.str() << "f"
//                << (int)list[i].pos.fret() << " note: "
//                << QString::fromStdString(list[i].note.getName());

  // generate m_fretFretList if needed
    m_fretFretList.clear();
    if (m_level->questionAs.isFret() && m_level->answersAs[TQAtype::e_asFretPos].isFret()) {
      QList<TfingerPos> tmpSameList;
      for (int i = 0; i < list.size(); i++) {
        tmpSameList.clear();
        getTheSamePos(list[i].pos, tmpSameList);
          if (!tmpSameList.isEmpty())
            m_fretFretList << (quint16)i;
      }
      qDebug() << "question list for only guitar created\nnumber:" << m_fretFretList.size() <<
          " amoung:" << list.size();
    }
    
    qsrand(QDateTime::currentDateTime().toTime_t());
    
    
    m_obligQuestNr = qMax(list.size() * 5, 20);
//     qDebug() << "question number:" << list.size();
    if (m_level->useKeySign && !m_level->isSingleKey)
        m_obligQuestNr = qMax(m_obligQuestNr, (m_level->hiKey.value() - m_level->loKey.value() + 1) * 5);
//     qDebug() << "possible x 5" << qaPossibilitys() * 5;
    m_obligQuestNr = qMax(qaPossibilitys() * 5, m_obligQuestNr);

}


Tnote TexecutorSupply::determineAccid(Tnote n) {
    Tnote nA = n;
    bool notFound = true;
    if (m_level->withSharps || m_level->withFlats || m_level->withDblAcc) {
        if (m_level->withDblAcc) {
            m_dblAccidsCntr++;
            if (m_dblAccidsCntr == 4) { //double accid note occurs every 4-th question
                if ( (qrand() % 2) ) // randomize dblSharp or dblFlat
                    nA = n.showWithDoubleSharp();
                else
                    nA = n.showWithDoubleFlat();
                if (nA == n) // dbl accids are not possible
                    m_dblAccidsCntr--;
                else {
                    m_dblAccidsCntr = 0;
                    notFound = false;
                }
            }
        }
        if (notFound && m_prevAccid != Tnote::e_Flat && m_level->withFlats) {
          if ((n.note == 3 || n.note == 7) && n.acidental == 0 ) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // fes or ces can occur every 3 e or b occurences
              m_eisCesCntr = 0;
              nA = n.showWithFlat();
              notFound = false;
            }
          } else { // other notes can be with flat allways
            nA = n.showWithFlat();
            notFound = false;
          }
        }
        if (notFound && m_prevAccid != Tnote::e_Sharp && m_level->withSharps) {
          if ((n.note == 4 || n.note == 1) && n.acidental == 0) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // eis or bis can occur every 3 f or c occurences
              nA = n.showWithSharp();
              m_eisCesCntr = 0;
            }
          } else { // other notes can be with sharp allways
            nA = n.showWithSharp();
          }
        }
    }
    m_prevAccid = (Tnote::Eacidentals)nA.acidental;
    return nA;
}

Tnote TexecutorSupply::forceEnharmAccid(Tnote n) {
    Tnote nX;
    char acc = m_prevAccid;
     for (int i = 0; i < 6; i++) { // to be sure that all cases are checked
        acc++;
        if (acc > 2) acc = -2;

        if (acc == Tnote::e_DoubleFlat && m_level->withDblAcc)
            nX = n.showWithDoubleFlat();
        if (acc == Tnote::e_Flat && m_level->withFlats)
            nX = n.showWithFlat();
        if (acc == Tnote::e_Natural)
            nX = n.showAsNatural();
        if (acc == Tnote::e_Sharp && m_level->withSharps)
            nX = n.showWithSharp();
        if (acc == Tnote::e_DoubleSharp && m_level->withDblAcc)
            nX = n.showWithDoubleSharp();
        if (nX.note && n != nX) break;
     }
    m_prevAccid = (Tnote::Eacidentals)acc;
    if (nX.note)
        return nX;
    else return n;
}

Tnote::EnameStyle TexecutorSupply::randomNameStyle(int style) {
    if (style != -1)
        if ((Tnote::EnameStyle)style == Tnote::e_italiano_Si)
            m_isSolfege = true;
        else
            m_isSolfege = false;
    if (m_isSolfege) {
        m_isSolfege = false;
        if (qrand() % 2) { // full name like cis, gisis
            if (gl->seventhIs_B)
                return Tnote::e_nederl_Bis;
            else
                return Tnote::e_deutsch_His;
        } else { // name and sign like c#, gx
            if (gl->seventhIs_B)
                return Tnote::e_english_Bb;
            else
                return Tnote::e_norsk_Hb;
        }
    } else {
        m_isSolfege = true;
        return Tnote::e_italiano_Si;
    }
}


void TexecutorSupply::getTheSamePos(TfingerPos& fingerPos, QList< TfingerPos >& posList, bool strCheck) {
  int chStr = gl->Gtune()[gl->strOrder(fingerPos.str()-1) + 1].getChromaticNrOfNote();
  for (int i = 0; i < 6; i++)
    if (i != gl->strOrder(fingerPos.str()-1)) { 
      if (strCheck && !m_level->usedStrings[i])
          continue; // skip unavailable strings when strCheck is true
      int fret = chStr + fingerPos.fret() - gl->Gtune()[gl->strOrder(i) + 1].getChromaticNrOfNote();
      if (fret >= m_level->loFret && fret <= m_level->hiFret) {
        posList << TfingerPos(gl->strOrder(i) + 1, fret);
      }
    }
}


void TexecutorSupply::calcQAPossibleCount() {
  m_qaPossib = 0;
  if (m_level->questionAs.isNote()) {
    if (m_level->answersAs[TQAtype::e_asNote].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asNote].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isName()) {
    if (m_level->answersAs[TQAtype::e_asName].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isFret()) {
    if (m_level->answersAs[TQAtype::e_asFretPos].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asFretPos].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isSound()) {
    if (m_level->answersAs[TQAtype::e_asSound].isNote())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isFret())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isSound())
      m_qaPossib++;
  }
}


//##########################################################################################
//#######################     EVENTS        ################################################
//##########################################################################################

bool TexecutorSupply::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (me->button() == Qt::RightButton) {
        emit rightButtonClicked();
        return true;   
    }
  }
  return QObject::eventFilter(obj, event);
}
