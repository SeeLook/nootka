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

#include <score/tsimplescore.h>
#include <QPointer>

class TnoteName;
class QPushButton;
class TscoreNote;
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
		
				/** Removes score controller from layout and returns a pointer to it.  */
		TscoreControl* getFreeController();
		
		int widthToHeight(int hi); /** Returns width of score when its height is @p hi. */
		
		void setNote(Tnote note);
		
				/** Describes insert mode of a score */
		enum EinMode {
			e_single, // single note mode
			e_multi, // editing/setting the last note adds next one
			e_record // calling setNote() from outside switches current index to the next note or adds one
		};
		
		EinMode insertMode() { return m_inMode; }
		void setInsertMode(EinMode mode);
		

//     void askQuestion(Tnote note, char realStr = 0);
//     void askQuestion(Tnote note, TkeySignature key, char realStr = 0);
//     void clearScore();
		
				/** It sets TkeySignatureView background to question color, sets fake key signature
        * and invokes askQuestion in TkeySignatureView */
//     void prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName);
		
        /** Connects or disconnects reactions for clicking a note
        * and showing enharmonic notes depends on is exam executing (disconnect)
        * or not (connect).*/
    void isExamExecuting(bool isIt);
		
        /** Internally it calls setScoreDisabled(false) to unlock and locks unused noteViews (1 & 2) again. */
//     void unLockScore();
//     void setKeyViewBg(QColor C);
//     void setNoteViewBg(int id, QColor C);
//     void forceAccidental(Tnote::Eacidentals accid); /** Sets given accidental */
		
        /** Marks note with border and blur. It is used to show was answer correct or not. */
//     void markAnswered(QColor blurColor);
//     void markQuestion(QColor blurColor);
		
		void enableAccidToKeyAnim(bool enable);
		bool isAccidToKeyAnimEnabled();
		
				/** Performs animation that transforming current selected note to given @p goodNote */
// 		void correctNote(Tnote& goodNote, const QColor& color);
		
				/** Performs rewinding of current key to @p newKey */
// 		void correctKeySignature(TkeySignature newKey);
// 		void correctAccidental(Tnote& goodNote);
		
				/** Displays note name of first note or the second if given next to its note-head in a score. */
// 		void showNames(Tnote::EnameStyle st, bool forAll = false);
		void deleteNoteName(int id); /** Deletes given instance of note name if it exist */
		
				/** Returns note head rectangle if visible or empty QRectF.  */
		QRectF noteRect(int noteNr);
			
				/** Position of a note in graphics view coordinates. */
		QPoint notePos(int noteNr);
		
signals:
		void noteChanged(int index, Tnote note);
		
        /** This signal is emitted during an exam when expert answers are used. */
    void noteClicked();
		
public slots:
    void whenNoteWasChanged(int index, Tnote note);
    void setScordature(); /** Sets scordature to value kept in Tglobal. */
    void noteWasClicked(int index);
		void noteWasSelected(int index);
		void expertNoteChanged();
		void onClefChanged(Tclef cl);

protected:
	virtual void resizeEvent(QResizeEvent* event);
	
	TscoreStaff* currentStaff();
		
protected slots:
				/** Refresh some things after switch scordature, notes state and color. */
		void onPianoSwitch();
// 		void strikeBlinkingFinished();
// 		void keyBlinkingFinished();
// 		void finishCorrection();
		void staffHasNoSpace(int staffNr); /** Create new (next) staff */
		void staffHasFreeSpace(int staffNr, int notesFree); /** Move notes to this staff from the next one */
		void noteGetsFree(int staffNr, TscoreNote* freeNote);
		void noteRemovingSlot(int staffNr, int noteToDel);
		void noteAddingSlot(int staffNr, int noteToDel);
		void zoomScoreSlot();
		void keyChangedSlot();
		void staffLoNoteChanged(int staffNr, qreal loNoteYoff);
		void staffHiNoteChanged(int staffNr, qreal hiNoteYoff);
		
		void showNameMenu(TscoreNote* sn);
		void menuChangedNote(Tnote n);
		
private:
		void restoreNotesSettings(); /** Sets notes colors according to globals. */
		void performScordatureSet(); /** Common method to set scordature */
		
				/** Creates QGraphicsRectItem with answer color, places it under the staff and adds to m_bgRects list.
				 * clearScore() removes it. */
		void createBgRect(QColor c, qreal width, QPointF pos);
    void checkAndAddNote(TscoreStaff* sendStaff, int noteIndex);
		
				/** Adds given staff at the end of m_staves list or creates a new one.
				 * Sets staff number corresponding to its index in the m_staves list,
				 * connects the staff with TmainScore slots */
		void addStaff(TscoreStaff* st = 0);
		void updateSceneRect(); /** Adjusts score scene to space required by staff(staves) */
		void adjustStaffWidth(TscoreStaff *st); /** Calls TscoreStaff::setViewWidth with score width  */
		void changeCurrentIndex(int newIndex);

private:
		QList<TscoreStaff*>					 m_staves; // list of staves in page view
		QGraphicsSimpleTextItem 		*m_questMark;
		QGraphicsTextItem 					*m_questKey;
		QList<TgraphicsTextTip*>		 m_noteName; // for now only two notes are used
		QList<QGraphicsRectItem*> 	 m_bgRects; // list of rectangles with highlights
		TstrikedOutItem 						*m_strikeOut;
    QPointer<TblinkingItem>      m_bliking, m_keyBlinking;;
		Tnote												 m_goodNote;
		TkeySignature								 m_goodKey;
		bool 												 m_showNameInCorrection;
		Tnote::EnameStyle						 m_corrStyle;
		EinMode											 m_inMode;
		QPushButton									*m_inZoomBuut, *m_outZoomButt;
				/** m_clickedOff indicates whether setNote() is set to m_currentIndex and whether to the next after current */
		int													 m_clickedOff, m_currentIndex;
		qreal												 m_scale;
		QPointer<TnoteName>					 m_nameMenu;
		QPointer<TscoreNote>				 m_currentNameSegment; /** Currently edited TscoreNote by menu. */
};

#endif // TMAINSCORE_H
