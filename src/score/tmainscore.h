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

#ifndef TMAINSCORE_H
#define TMAINSCORE_H

#include "tsimplescore.h"
#include <QPointer>

class TgraphicsTextTip;
class TblinkingItem;
class TstrikedOutItem;
class QGraphicsRectItem;
class QGraphicsTextItem;
class QGraphicsSimpleTextItem;

/** 
 * This is a main score of Nootka.
 * In "normal" mode it menages enharmonic, double accidentals and so on
 * In exam mode it responses for locking/unlocking, backgrounds, question marks.
 */
class TmainScore : public TsimpleScore
{
    Q_OBJECT

public:
    TmainScore(QWidget *parent = 0);
    ~TmainScore();
		
		
		void setEnableEnharmNotes(bool isEnabled);
    void acceptSettings();
    void askQuestion(Tnote note, char realStr = 0);
    void askQuestion(Tnote note, TkeySignature key, char realStr = 0);
    void clearScore();
		
				/** Sets scordature to value kept in Tglobal. */
    void setScordature();
		
				/** It sets TkeySignatureView background to question color, sets fake key signature
        * and invokes askQuestion in TkeySignatureView */
    void prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName);
		
        /** Connects or disconnects reactions for clicking a note
        * and showing enharmonic notes depends on is exam executing (disconnect)
        * or not (connect).*/
    void isExamExecuting(bool isIt);
		
        /** Internally it calls setScoreDisabled(false) to unlock and locks unused noteViews (1 & 2) again. */
    void unLockScore();
    void setKeyViewBg(QColor C);
    void setNoteViewBg(int id, QColor C);
    void forceAccidental(Tnote::Eacidentals accid); /** Sets given accidental */
		
        /** Marks note with border and blur. It is used to show was answer correct or not. */
    void markAnswered(QColor blurColor);
    void markQuestion(QColor blurColor);
		
		void enableAccidToKeyAnim(bool enable);
		bool isAccidToKeyAnimEnabled();
		
				/** Removes score controller from layout and returns a pointer to it.  */
		TscoreControl* getFreeController();
		
				/** Performs animation that transforming current selected note to given @p goodNote */
		void correctNote(Tnote& goodNote, const QColor& color);
		
				/** Performs rewinding of current key to @p newKey */
		void correctKeySignature(TkeySignature newKey);
		void correctAccidental(Tnote& goodNote);
		
				/** Displays note name of note (first) or all if @p forAll is true 
				 * next to its note-head in a score. */
		void showNames(bool forAll = false);
		void deleteNoteName(int id); /** Deletes given instance of note name if it exist */
		
				/** Returns note head rectangle if visible or empty QRectF.  */
		QRectF noteRect(int noteNr);
			
				/** Position of a note in graphics view coordinates. */
		QPoint notePos(int noteNr);
		
		int widthToHeight(int hi); /** Returns width of score when its height is @p hi. */
		
signals:
		void noteChanged(int index, Tnote note);
		
        /** This signal is emitted during an exam when expert answers are used. */
    void noteClicked();
		
public slots:
    void whenNoteWasChanged(int index, Tnote note);
    void expertNoteChanged();

		
protected slots:
				/** Refresh some things after switch scordature, notes state and color. */
		void onPianoSwitch();
		void strikeBlinkingFinished();
		void keyBlinkingFinished();
		void finishCorrection();
		
private:
		void restoreNotesSettings(); /** Sets notes colors according to globals. */
		
				/** Creates QGraphicsRectItem with answer color, places it under the staff and adds to m_bgRects list.
				 * clearScore() removes it. */
		void createBgRect(QColor c, qreal width, QPointF pos);

private:
		QGraphicsSimpleTextItem 		*m_questMark;
		QGraphicsTextItem 					*m_questKey;
		QList<TgraphicsTextTip*>		 m_noteName; // for now only two notes are used
		QList<QGraphicsRectItem*> 	 m_bgRects; // list of rectangles with highlights
		TstrikedOutItem 						*m_strikeOut;
    QPointer<TblinkingItem>      m_bliking, m_keyBlinking;;
		Tnote												 m_goodNote;
		TkeySignature								 m_goodKey;
		bool 												 m_showNameInCorrection;
};

#endif // TMAINSCORE_H
