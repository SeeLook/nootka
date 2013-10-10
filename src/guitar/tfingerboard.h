/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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

#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

#include "tnote.h"
#include "tfingerpos.h"
#include <QWidget>
#include <QGraphicsView>

class TanimedItem;
class TstrikedOutItem;
class TgraphicsTextTip;

class TfingerBoard : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TfingerBoard(QWidget *parent = 0);

    void acceptSettings();

        /** It paints fingerprint on the fretboard in place where note is.
        * If globals GshowOtherPos is true all possibilities are shown.
        * If @param realStr is set is shown only on pointed string (if any).
        * It returns true if something is shown.*/
    void setFinger(Tnote note);
    void setFinger(TfingerPos pos);
        /** Returns selected finger position */
    TfingerPos getfingerPos() { return m_fingerPos; }

    void askQuestion(TfingerPos pos);
    void clearFingerBoard();
        /** Creates rectriangle (or two) indicates range of frets in exam.*/
    void createRangeBox(char loFret, char hiFret);
    void prepareAnswer();
    void deleteRangeBox();
    void setGuitarDisabled(bool disabled);
        /** Highlights given string */
    void setHighlitedString(char realStrNr);
    void clearHighLight();
        /** Returns x coordinate of 12th fret where guitar body starts. */
    int posX12fret();
				/** Guitar fingerboard rectriangle */
		QRect fbRect() { return m_fbRect; }
        /** Returns width of a string. */
    qreal stringWidth(int strNr) { return m_strWidth[qBound(1, strNr + 1, 6)]; }
        /** Returns @param true when cursor is over the widget. */
    bool isCursorOverGuitar() { return m_isCursorOverGuitar; }
        /** Marks selected string or fret. Marking is cleaned when clearFingerBoard() is invoked. */
    void markAnswer(QColor blurColor);
        /** Highlights m_questString or m_questFinger after answering. */
    void markQuestion(QColor blurColor);
		void correctPosition(TfingerPos &pos, const QColor color);

signals:
    void guitarClicked(Tnote note);


protected:
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);
    bool event(QEvent *event);
    
    void paint();
    Tnote posToNote(int str, int fret);
				/** Determines string width by its note pitch. Sets loNote & hiNote */
		void setTune();

private:
        /** Represents top left positions and size of a fingerboard */
    QRect m_fbRect;
        /** Distance between strings */
    short m_strGap;
        /** Average width of fret */
    short m_fretWidth;
        /** Position of the last fret (in whole widget coordinates) */
    short lastFret;
        /** Actual position of cursor over the guitar in strings/frets coordinates */
    short m_curStr, m_curFret;
				/** Chromatic numbers of lowest note in tune and highest. */
		short m_loNote, m_hiNote;
        /** It keeps position of selected fingerprint.*/
    TfingerPos m_fingerPos;
        /** @param fretsPos  stores X positions of frets in global widget coordinates */
    short m_fretsPos[24];
        /** Array of width each string. Width depends on fretboard height. */
    qreal m_strWidth[6];
				/** Base values from which m_strWidth is calculated determined from tune. */
		qreal m_widthFromPitch[6];
		QColor m_strColors[6];

    QGraphicsScene *m_scene;
    QGraphicsEllipseItem *m_workFinger, *m_fingers[6], *m_questFinger;
    int m_strNr, m_fretNr;
    QGraphicsLineItem *m_workStrings[6], *m_strings[6], *m_questString, *m_highString;
    QGraphicsSimpleTextItem *m_questMark;
				/** Tip about a note is imposible to show with current tune. */
		TgraphicsTextTip *m_beyondTip;
        /** Keeps selected note*/
    Tnote m_selNote;
        /** Position from a question - is needed to calculate size of questioned finger
        * or string if naughty user changes window size. */
    TfingerPos m_questPos;
        /** Frets range in an exam*/
    char m_loFret, m_hiFret;
    QGraphicsRectItem *m_rangeBox1, *m_rangeBox2;
    bool m_isDisabled;
    int m_hilightedStrNr;
    bool m_isCursorOverGuitar;
		TfingerPos m_goodPos;
		TstrikedOutItem *m_strikeOut;
		TanimedItem *m_animation;
		QGraphicsItem *m_movingItem; // string line during animation

private:
    void paintFinger(QGraphicsEllipseItem *f, char strNr, char fretNr);
    void paintQuestMark();
    void resizeRangeBox();
    void paintFingerAtPoint(QPoint p);
		void deleteBeyondTip();
		
private slots:
		void strikeBlinkingFinished();
		void finishCorrection();


};

#endif // TFINGERBOARD_H
