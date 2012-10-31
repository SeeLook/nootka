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

#ifndef TSCOREWIDGET_H
#define TSCOREWIDGET_H

#include "tscorewidgetsimple.h"

class TscordatureView;
class Tnote;
class TkeySignature;
class QGraphicsSimpleTextItem;
class QGraphicsTextItem;
/** Asking questions mechanism works as follow:
  * @fn askQuestion sets a note, string number (if declared) and key signature (if enabled).
  * @fn setKeyViewBg and @fn setNoteViewBg set backgrounds of asked note TnoteView[1]
  * and key to gl->EquestionColor. Also question mark is drown on TnoteView[2] (m_questMark)
  * If key has to be given as answer @fn prepareKeyToAnswer is invoked, fake key is set
  * and background color is gl->EanswerColor.
  * When note is an answer TnoteView[0] background is set to gl->EanswerColor.
  * Also if the accidental is required @fn forceAccidental is called.
  * Then widget is unlocked by @fn unLockScore.
  * After all @fn clearScore clean them.
@author Tomasz Bojczuk  <tomaszbojczuk@gmail.com>
*
*/
class TscoreWidget : public TscoreWidgetSimple
{
    Q_OBJECT
public:
    explicit TscoreWidget(unsigned char _notesCount = 1, QWidget* parent = 0);

    void setEnableEnharmNotes(bool isEnabled);
    void acceptSettings();
    void askQuestion(Tnote note, char realStr = 0);
    void askQuestion(Tnote note, TkeySignature key, char realStr = 0);
    void clearScore();
        /** Sets scordature to value keeps in Tglobal. */
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

signals:
    void noteChanged(int index, Tnote note);
        /** This signal is emited during an exam when expert answers are used. */
    void noteClicked();

public slots:
    void whenNoteWasChanged(int index, Tnote note);
    void expertNoteChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    
private:
    QGraphicsSimpleTextItem *m_questMark;
    QGraphicsTextItem *m_questKey;
    TscordatureView *m_scordature;
    void resizeQuestMark();
    void resizeKeyText();


};

#endif // TSCOREWIDGET_H
