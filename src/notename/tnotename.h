/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#ifndef TNOTENAME_H
#define TNOTENAME_H

#include <QWidget>
#include <music/tnote.h>
#include <tcolor.h>

class QMenu;
class QGraphicsTextItem;
class QHBoxLayout;
class TnoteNameLabel;
class QPushButton;
class TpushButton;
class QButtonGroup;


/**
 * This widget displays note name and buttons to manipulate it.
 * Since Nootka 1.1.0 it is squeezed into QMenu, so it exist temporary
 * It has to be invoked with exec() method
 */
class TnoteName : public QWidget
{
    Q_OBJECT
public:
    explicit TnoteName(QWidget *parent = 0);
		virtual ~TnoteName();

    static const char * const octaves[8];
    static const char * const octavesFull[8];
		
				/** Displays note name as menu. @p pos is desired position in screen coordinates
				 * @p scoreFactor is m11() factor of a scene to automatically calculate and place the menu
				 * on the right or left side of given position */
		void exec(QPoint pos, qreal scoreFactor);

        /** Sets names on buttons to given style.  Doesn't refresh note name label. */
    void setNoteNamesOnButt(Tnote::EnameStyle nameStyle);
    void setEnabledDblAccid(bool isEnabled);
    void setEnabledEnharmNotes(bool isEnabled);
    void setNoteName(Tnote note);
    void setNoteName(TnotesList notes);
    Tnote getNoteName() {return m_notes[0]; }
    Tnote getNoteName(int index) {return m_notes[index]; }
    void resize(int fontSize = 0);
    void setAmbitus(Tnote lo, Tnote hi);
    void askQuestion(Tnote note, Tnote::EnameStyle questStyle, char strNr = 0);
    void prepAnswer(Tnote::EnameStyle answStyle);
    void setNameDisabled(bool isDisabled);
    void clearNoteName();
    void setStyle(Tnote::EnameStyle style); // Sets style. Doesn't refresh name label
    Tnote::EnameStyle style() { return m_style; } // Style used in note name
    
        /** Marks m_nameLabel with given color. When clearNoteName() is invoked - marks are cleared. */
    void markNameLabel(QColor markColor);
		
				/** Highlights and check given accid button   */
		void forceAccidental(char accid);
		
				/** @p isWrong  determines kind of animation performed after invoking this method. */
		void correctName(Tnote &goodName, const QColor &color, bool isWrong = true);
		
				/** Returns given color mixed with palette base and 220 of alpha. */
		QColor prepareBgColor(const QColor &halfColor);
		
		QRectF textRect(); /** Rectangle of note name text item */
		QPoint textPos(); /** Position of name text in main window coordinates. */
		QRect labelRect(); /** Name label position and size in TnoteName coordinates. */		

signals:
    void noteNameWasChanged(Tnote note);
    void noteButtonClicked();
		

protected:
    void resizeEvent(QResizeEvent *);

private:
    TnoteNameLabel				*m_nameLabel;
    TpushButton 					*m_noteButtons[7];
    TpushButton 					*m_octaveButtons[8];
    TpushButton 					*m_dblFlatButt, *m_flatButt, *m_sharpButt, *m_dblSharpButt;
    QButtonGroup 					*m_noteGroup, *m_octaveGroup;
    int 									m_prevOctButton; /** Keeps index of previous selected octave button, none if -1 */
    static 								Tnote::EnameStyle m_style;

    TnotesList 						m_notes;
    short 								m_ambitMin, m_ambitMax;
		Tnote 								m_goodNote;
		int 									m_blinkingPhase;
		QMenu									*m_menu;
		qreal									m_scoreFactor; //* Size coefficient of a score displaying this note name (menu) */
    
private:
    void setNoteName(char noteNr, char octNr, char accNr);
    void setNameText();
		
				/** Sets note, accid and octave buttons according to given note. */
    void setButtons(Tnote note);
		
				/** Presses accidental button or uncheck them all if accid none (0).  */
		void checkAccidButtons(char accid);
    void uncheckAccidButtons();
    void uncheckAllButtons();
		
				/** Returns current state of accid buttons converted to accidental value [-2 to 2] */
		char getSelectedAccid();
		

private slots:
    void noteWasChanged(int noteNr);
    void accidWasChanged();
    void octaveWasChanged(int octNr);
    void correctAnimationFinished();
    void invokeBlinkingAgain();
       

};

#endif // TNOTENAME_H
