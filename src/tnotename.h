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

#ifndef TNOTENAME_H
#define TNOTENAME_H

#include <QWidget>
#include "tnote.h"

class QLabel;
class TpushButton;
class QButtonGroup;

class TnoteName : public QWidget
{
    Q_OBJECT
public:
    explicit TnoteName(QWidget *parent = 0);

    static const char * const octaves[6];
    static const char * const octavesFull[6];

    void setNoteNamesOnButt(Tnote::EnameStyle nameStyle);
    void setEnabledDblAccid(bool isEnabled);
    void setEnabledEnharmNotes(bool isEnabled);
    void setNoteName(Tnote note);
    void setNoteName(TnotesList notes);
    Tnote getNoteName() {return m_notes[0]; }
    Tnote getNoteName(int index) {return m_notes[index]; }
    void resize(int fontSize = 0);
    void setAmbitus(Tnote lo, Tnote hi);
    static QString noteToRichText(Tnote note);
    void askQuestion(Tnote note, char strNr = 0);
    void prepAnswer(Tnote backNote = Tnote(0,0,0));
    void setNameDisabled(bool isDisabled);
    void clearNoteName();
//    void setButtonsWithOctaves

signals:
    void noteNameWasChanged(Tnote note);
    void noteButtonClicked();

public slots:

private:
    QLabel *nameLabel;
    TpushButton *noteButtons[7];
    TpushButton *octaveButtons[6];
    TpushButton *dblFlatButt, *flatButt, *sharpButt, *dblSharpButt;
    QButtonGroup *noteGroup, *octaveGroup;
		/** Keeps index of previous selected octave Button.
		 * none if -1 */
	int m_prevOctButton;

    TnotesList m_notes;
    short m_ambitMin, m_ambitMax;
    void setNoteName(char noteNr, char octNr, char accNr);
    void setNameText();
    void setButtons(Tnote note);
    void uncheckAccidButtons();
    void  uncheckAllButtons();

private slots:
    void noteWasChanged(int noteNr);
    void accidWasChanged();
    void octaveWasChanged(int octNr);


};

#endif // TNOTENAME_H
