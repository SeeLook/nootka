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

#ifndef TEXECUTORSUPPLY_H
#define TEXECUTORSUPPLY_H

#include "tnote.h"
#include <QObject>
#include "tqaunit.h"

class TexamLevel;

/** A purpose of this class is to make the exam executor smaller.
 * Also it has eventFilter() reimplementation to manage right mouse button,
 * witch can not be simply captured by contextMenuEvent from disabled widgets. */
class TexecutorSupply : public QObject
{
  Q_OBJECT
public:
  TexecutorSupply(TexamLevel *level, QObject *parent = 0);
  
  void createQuestionsList(QList<TQAunit::TQAgroup> &list);
  Tnote determineAccid(Tnote n);
        /** */
  Tnote forceEnharmAccid(Tnote n);
  Tnote::EnameStyle randomNameStyle();
    /** Returns list with possible positions of a note on given position.
     * Given fingerPos (position) is not included.
     * If strCheck is true it is excluding strings unvailable in a level. */
  void getTheSamePos(TfingerPos &fingerPos, QList<TfingerPos> &posList, bool strCheck = true);
    /** Returns randomized number in questions list proper for question and answer on the guitar. */
  quint16 getQAnrForGuitarOnly() { return m_fretFretList[qrand() % m_fretFretList.size()]; }
    /** Returns true when qustions and answers as guitar are possible. */
  bool isGuitarOnlyPossible() { return !m_fretFretList.isEmpty(); }
    /** Useing level settings calculates number of mandatory questions in exam.
     * It is stored in m_totalQuestNr. */
  int obligQuestions() { return m_obligQuestNr; }
  int qaPossibilitys() { return m_qaPoossib; }
  
  
signals:
  void rightButtonClicked();
  
protected:
  virtual bool eventFilter(QObject* obj, QEvent* event);
      /** Calculates how many question/answer combinations are posible fffor different types
       * enabled in a level. */
  void calcQAPossibleCount();
      /** Displays message when exam can be finished. */
  void examFinished();
  
private:
  TexamLevel *m_level;
      /** Total number of questions to answer in exam (without penaltys) */
  int m_obligQuestNr;
  int m_qaPoossib;
      /** Previous accidental used. */
  Tnote::Eacidentals m_prevAccid;
      /** intervals between asking about double accidentals.
        * By default every forth question is with double adccid.*/
  int m_dblAccidsCntr;
      /** It counts occurency of e, f, b and c notes
       * to avoid asking about eis fes his and ces to oftent 
       * witch occurs in scores rerely. */
  int m_eisCesCntr;
        /** to switch nameing style between "do re mi" and "c d e"
        * when question and answer are note name.*/
  bool m_isSolfege;
      /** List of numbers of questions in the questions list that can be 
       * either question as position or answer. 
       * The list is initialised in createQuestionsList() method if level has that settings. */
  QList<quint16> m_fretFretList;
      
};

#endif // TEXECUTORSUPPLY_H
