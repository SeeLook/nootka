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

#ifndef TMAINSCORE_H
#define TMAINSCORE_H

#include "tsimplescore.h"

class QGraphicsTextItem;
class QGraphicsSimpleTextItem;

/** 
 * This is a main score of Nootka.
 * In "normal" mode it menages enharmonics, double accidentals and so on
 * In exam mode it responses for locking/unlocking, backrounds, question marks.
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
        * and showing enharmonics notes depends on is exam executing (disconnect)
        * or not (connect).*/
    void isExamExecuting(bool isIt);
        /** Internally it calls setScoreDisabled(false) to unlock
        * and locks unused noteViews (1 & 2) again. */
    void unLockScore();
    void setKeyViewBg(QColor C);
    void setNoteViewBg(int id, QColor C);
        /** Sets given accidental*/
    void forceAccidental(Tnote::Eacidentals accid);
        /** Marks note with border and blur.
         * it is used to show was answer correct or not. */
    void markAnswered(QColor blurColor);
    void markQuestion(QColor blurColor);
		
signals:
		void noteChanged(int index, Tnote note);
        /** This signal is emited during an exam when expert answers are used. */
    void noteClicked();
		
public slots:
    void whenNoteWasChanged(int index, Tnote note);
    void expertNoteChanged();

		
protected slots:
				/** Refresh some things after switch 
				 * scordature, notes state and color. */
		void onPianoSwitch();
		
private:
				/** Sets notes colors and readOnlyness according to globals. */
		void restoreNotesSettings();

private:
		QGraphicsSimpleTextItem *m_questMark;
		QGraphicsTextItem *m_questKey;

};

#endif // TMAINSCORE_H
