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

#ifndef TSCOREWIDGETSIMPLE_H
#define TSCOREWIDGETSIMPLE_H

#include <QWidget>
#include "tnote.h"
#include "tpushbutton.h"

class TnoteView;
class TkeySignatureView;
class Tnote;
class TkeySignature;
class TclefView;
class QPushButton;


/** Creates widget with score, and buttons to add/remove accids.
* @param _notesCount is count of notes in staff
* @param enableKeySign if @p TRUE - user can change it by mouse wheel
*/
class TscoreWidgetSimple : public QWidget
{

    friend class TkeySignatureView;
    friend class TnoteView;

    Q_OBJECT
public:
    TscoreWidgetSimple(unsigned char _notesCount = 1, QWidget *parent = 0);

        /** This metods get and set the key signature, and can be:
          * @li 0 - C-maj (a-min)
          * @li 1 - G-maj and so on up to 7 (for Cis-maj)
          * @li -1 - F-maj (d-min) and so on down to -7 (for Ges-maj)    */
    TkeySignature keySignature();
    void setKeySignature(TkeySignature keySign);

    QList <TnoteView*> noteViews;
    TkeySignatureView *keySignView;

        /** coefficient of staff's lines distance */
    short coeff;
    void setEnabledDblAccid(bool isEnabled);
    bool isDblAccidEnabled()
        {if (dblAccidFuse == 2) return true;
            else return false;}
    void setEnableKeySign(bool isEnabled);
    void setNote(int index, Tnote note);
        /** It returns @p note with hope that index poits existed Tnote element*/
    Tnote getNote(int index) { return m_notes[index]; }
        /** It hides pionted note and sets to Tnote(0,0,0)*/
    void clearNote(int index);
        /** Sets the lowest and the highest note in all TnoteViews instances.*/
    void setAmbitus(Tnote lo, Tnote hi);
        /** The same but for @param index TnoteView.*/
    void setAmbitus(Tnote lo, Tnote hi, int index);
        /** Keeps true when TscoreWidgetSimple has scordature.
        * Resize() uses it to calculate offset. */
    void setHasScord(bool has) { m_hasScord = has; }
        /** It disables accids buttons and locks editing,
        * but dasn't make score gray like standard setDisabled() method. */
    void setScoreDisabled(bool disabled);


signals:
    void noteHasChanged(int index, Tnote note);

public slots:
        /** Is called when global naming parameters are changed*/
    void refreshKeySignNameStyle();
    void noteWasClicked(int index);

protected:
    void paintEvent (QPaintEvent *event);
    void resizeEvent(QResizeEvent *);

    void setButtons(QPushButton *button);
                /** This array keeps accid for every note in key signature.
                 * @li 1 is sharp : keyAcidsArr[0] = 1  -> C# in key signature.
                 * @li -1 is flat : keyAcidsArr[7] = -1 -> Hb in key signature.
                 * @li 0 in none */
    char accInKeyArr[7];
    char dblAccidFuse;


protected slots:
    void onAcidButtonPressed();
    /** This slot is connected with signal emited by wheelEvent in TnoteView widget
      * and check or uncheck apropirate buttons. */
    void changeAccidButtonsState(int accNr);
    void onKeySignatureChanged();

private:
        /** Refreshs state of ScoreCanvWidth and coeff, after resizing */
    void resize();
        /** Returns Y position of note in TnoteView "unuts". */
    int getNotePos(Tnote note);

    QPushButton *m_sharpBut, *m_flatBut, *m_dblSharpBut;
	TpushButton *m_dblFlatBut;
    TnotesList m_notes;
    bool m_hasScord;
    TclefView *m_clef;


};

#endif // TSCOREWIDGETSIMPLE_H
