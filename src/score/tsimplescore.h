/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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
 

#ifndef TSIMPLESCORE_H
#define TSIMPLESCORE_H

#include <QWidget>
#include <music/tclef.h>
#include <music/tnote.h>
#include <music/tkeysignature.h>

class QGraphicsSimpleTextItem;
class TscoreView;
class QGraphicsView;
class TscoreControl;
class TscoreStaff;
class TscoreScene;


/** This class implements score.  */
class TsimpleScore : public QWidget
{
  Q_OBJECT
  
public:
    TsimpleScore(int notesNumber, QWidget *parent = 0, bool controler = true);
    ~TsimpleScore();

		void setNote(int index, Tnote note);
    Tnote getNote(int index); /** It returns @p note with hope that index points existed Tnote element. */
    void clearNote(int index); /** It hides pointed note and sets note to Tnote(0,0,0)*/
		void setStringNumber(int index, int realNr); /** Adds string number @p realNr to note @p index.  */
		void clearStringNumber(int index); /** Removes string number from note @p index. */
		void setNoteDisabled(int index, bool isDisabled); /** Disables/enables a note. */
		bool isNoteDisabled(int index);
		
		void setClef(Tclef clef);
		Tclef clef();
		void setClefDisabled(bool isDisabled);
		

				/** This method returns the key signature if score has got TscoreKeySign item, 
				 * key can be:
         * @li 0 - C-maj (a-min)
         * @li 1 - G-maj and so on up to 7 (for Cis-maj)
         * @li -1 - F-maj (d-min) and so on down to -7 (for Ges-maj)    
				 * When TscoreKeySign doesn't exist 0: - a-min/C-maj is returned. */
    TkeySignature keySignature();
    void setKeySignature(TkeySignature keySign);
		void setEnableKeySign(bool isEnabled);
		
		void setEnabledDblAccid(bool isEnabled);
		
				/** Sets TsimpleScore to piano staff view. Clefs are readOnly. */
		void setPianoStaff(bool isPiano);
		bool isPianoStaff() { return m_isPianoStaff; }
		
				/** Sets background color. Alpha value will be overriden. */
    void setBGcolor(QColor bgColor);
		
				/** It disables accids buttons and locks editing,
					* but doesn't make score gray like standard setDisabled() method. */
    void setScoreDisabled(bool disabled);
		
				/** Sets the lowest and the highest note in all note segments on the score .*/
    void setAmbitus(Tnote lo, Tnote hi);
		
        /** Sets the lowest and the highest note for @p index of note segment .*/
    void setAmbitus(int index, Tnote lo, Tnote hi);
		
		Tnote lowestNote(); /** Returns lowest possible note on the staff in current clef */
		Tnote highestNote(); /** Returns highest possible note on the staff in current clef */
		void addBGglyph(int instr); /** Adds background with glyph identified  kind of instrument. */
		
// 		virtual QSize sizeHint() const;
// 		virtual int heightForWidth(int w) const;
		
signals:
				/** As long as QGraphicsScene items haven't got status tips TscoreItems has its own mechanism of tips.
				 * This signal is emitted when any TscoreScene element gots hoverEnterEvent 
				 * with status tip to display. */
    void statusTip(QString);
		void noteWasChanged(int index, Tnote note);
		
				/** TsimpleScore takes care about changing staves but also emits this signal when changes are done.*/
		void pianoStaffSwitched();
		void clefChanged(Tclef);
		
public slots:
		void noteWasClicked(int index);
		
protected:
		bool layoutHasControl; // keeps true when TsimpleScore layout has score controller
		TscoreView* score() { return m_score; }
		TscoreScene* scene() { return m_scene; }
		
				/** Pointer to TscoreStaff should never go to public !!!!
				 * TscoreStaff instance is changeable by pianoStaffSwitched() and any connection with its signals will be lost.
				 * Inner methods of TsimpleScore take care about refreshing 
				 * connection with newer staff instances and this class emits appropirate signals. */
		TscoreStaff* staff() { return m_staff; }
		TscoreControl* scoreController() { return m_scoreControl; }
		qreal pianoFactor() { return m_pianoFactor; }
    
    
protected slots:
				/** It grabs TscoreItems statusTips and generates QStatusTipEvent fof parent widget. */
    void statusTipChanged(QString status);
		
				/** Except response for scaling TscoreView widget to according to new height,
				 * this method takes care about new width of the score. 
				 * It is necessary to call it after staff width changed f.e by:
				 * setPianoStaff(), setEnableKeySign() and setScordature()				 */
    void resizeEvent(QResizeEvent* event);
		
				/** This is response for user demand to change to or from piano staff. */
		void switchToPianoStaff(Tclef clef);
		void onClefChanged(Tclef clef);
  
private:
    TscoreScene     						*m_scene;
    TscoreStaff     						*m_staff, *m_lowerStaff;
    TscoreControl   						*m_scoreControl;
    TscoreView		  						*m_score;
		QGraphicsSimpleTextItem 		*m_bgGlyph;
		bool 												m_isPianoStaff;
		int 												m_notesNr;
		qreal 											m_pianoFactor; // factor of a score size depends on is it piano staff(smaller) or not
  
};

#endif // TSIMPLESCORE_H
