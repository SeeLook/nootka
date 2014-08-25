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

#ifndef TEXECUTORSUPPLY_H
#define TEXECUTORSUPPLY_H

#include <music/tnote.h>
#include <exam/tqaunit.h>


class MainWindow;
class Texam;
class QWidget;
class Tlevel;


/** 
 * A purpose of this class is to make the exam executor smaller.
 * Also it has eventFilter() re-implementation to manage right mouse button,
 * witch can not be simply captured by contextMenuEvent from disabled widgets. 
 */
class TexecutorSupply : public QObject
{
  Q_OBJECT
public:
  TexecutorSupply(Tlevel *level, QObject *parent = 0);
  
  void createQuestionsList(QList<TQAunit::TQAgroup> &list);
  Tnote determineAccid(Tnote n);
 
  Tnote forceEnharmAccid(Tnote n);
	
			/** Switches given style to opposite (letters or solfege).
			* If none specified, gives opposite to previous returned. */
  Tnote::EnameStyle randomNameStyle(int style = -1);
	
			/** Returns list with possible positions of a note on given position.
			* Given fingerPos (position) is not included.
			* If strCheck is true it is excluding strings unavailable in a level.
			* @p order determines whether string order is respected. */
  void getTheSamePos(TfingerPos &fingerPos, QList<TfingerPos> &posList, bool strCheck = true, bool order = true);
	
			/** The same as getTheSamePos() but string order is ignored */
	void getTheSamePosNoOrder(TfingerPos &fingerPos, QList<TfingerPos> &posList, bool strCheck = true);
	
			/** Returns randomized number in questions list proper for question and answer on the guitar. */
  quint16 getQAnrForGuitarOnly() { return m_fretFretList[qrand() % m_fretFretList.size()]; }
  
			/** Returns true when questions and answers as guitar are possible. */
  bool isGuitarOnlyPossible() { return !m_fretFretList.isEmpty(); }
  
			/** Using level settings calculates number of mandatory questions in exam.
			* It is stored in m_totalQuestNr. */
  int obligQuestions() { return m_obligQuestNr; }
  int qaPossibilities() { return m_qaPossib; }
  
  void setFinished(bool wasFinished = true) { m_wasFinished = wasFinished;}
  bool wasFinished() { return m_wasFinished; }
			
			/** Returns @p True when level and user instrument preferences determine corrected answer to be played.
			 * When corrections are possible on guitar it returns @p False */
  static bool isCorrectedPlayable() { return m_playCorrections; }
		
			/** Check. are current guitar fret number and tuning different than exercise or exam level.
			 * Adjust globals (tune and fret number have to be stored before) and
			 * Displays message about changes, if any. */
  static void checkGuitarParamsChanged(MainWindow* parent, Texam* exam);
		
			/** It is true when checkGuitarParamsChanged() obtained message to display.
			 * It is used to inform MainWindow that status label is busy by the message. */
	static bool paramsChangedMessage() { return m_paramsMessage; }
	static QColor& answerColor(const TQAunit& answer); /** Returns color depends on answer type. */
	static QColor& answerColor(quint8 mistake); /** Returns color depends on answer type. */
	
	int loFret() { return m_loFret; }
	int hiFret() { return m_hiFret; }
	
			/** Checks are given notes matched. Result (mistake(s)) is written into @p curQ.
			 * When @p reqOctave is false octaves check are skipped
			 * When @p reqAccid is false Cis matches to Des and etc...	 */
	void checkNotes(TQAunit& curQ, Tnote& expectedNote, Tnote& userNote, bool reqOctave, bool reqAccid);
	
			/** Compares given melodies respecting settings in current level. Saves results into Tattempt */
	void compareMelodies(Tmelody* q, Tmelody* a, Tattempt* att);
  
  
signals:
  void rightButtonClicked();
  
protected:
	
  virtual bool eventFilter(QObject* obj, QEvent* event);
	
      /** Calculates how many question/answer combinations are possible for different types enabled in a level. */
  void calcQAPossibleCount();
		
			/** Determines should be corrections played or shown on the guitar */
	static void checkPlayCorrected(Tlevel* level);
			
	
private:
			/** Checks is given note in level key signature (if single key) 
			 * or in some key in key signature range */
	bool isNoteInKey(Tnote &n);
	
			/** Appends given note and fret position to question list. */
	void addToList(QList<TQAunit::TQAgroup> &list, Tnote &n, TfingerPos &f);
	
			/** Returns number of string - itself (realNr - 1) when strings order is not respected
			 * or pitch-ordered taken from gl->strOrder() */
	quint8 strNr(quint8 str0to6, bool ordered = true);
  
private:
  Tlevel 									*m_level;
  int 										m_obligQuestNr; /** Total number of questions to answer in exam (without penalties) */
  int 										m_qaPossib;
	Tnote::Eacidentals 			m_prevAccid; /** Previous accidental used. */
	
      /** intervals between asking about double accidentals.
        * By default every forth question is with double accidentals.*/
  int 										m_dblAccidsCntr;
		
      /** It counts occurrence of e, f, b and c notes
       * to avoid asking about eis fes his and ces to often 
       * witch occurs in scores rarely. */
  int 										m_eisCesCntr;
	
        /** to switch naming style between "do re mi" and "c d e"
        * when question and answer are note name.*/
  bool 										m_isSolfege;
	
      /** List of numbers of questions in the questions list that can be 
       * either question as position or answer. 
       * The list is initialized in createQuestionsList() method if level has that settings. */
  QList<quint16> 					m_fretFretList;
	
  bool 										m_wasFinished; /** True when message about finished exam was shown. */
  static bool							m_playCorrections; /** Corrected answers will be played (True) or shown on the guitar (False) */
	static bool							m_paramsMessage; /** True when message was displayed */
			
			/** Temporary stored fret range of a level. 
			 * Values may be differ than originally in level when level no needs guitar
			 * but to show fret range they are adjusted. 
			 * LEVEL VALUES REMAINED UNTOUCHED */
	int 										m_loFret, m_hiFret;
      
};

#endif // TEXECUTORSUPPLY_H
