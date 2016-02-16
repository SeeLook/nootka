/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   			      			                                 *
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


#ifndef TNOTATIONRADIOGROUP_H
#define TNOTATIONRADIOGROUP_H

#include <music/tnote.h>
#include <QGroupBox>

class QRadioButton;
class QButtonGroup;

class TnotationRadioGroup: public QGroupBox
{
        Q_OBJECT
public:
				/** When lettersAndSolfege is true widget is divided on two columns and buttons are in separate QButtonGroup
				 * for letter and for solfege (Note name settings). 
				 * When false there is single column and single QButtonGroup (Key signature name settings). */
    explicit TnotationRadioGroup(Tnote::EnameStyle _notation, bool lettersAndSolfege = true, QWidget *parent = 0);


    Tnote::EnameStyle notation;

    static QString strNorsk() { return tr("Scandinavian"); }
    static QString strItal() { return tr("Italian"); }
    static QString strDeutsch() { return tr("German"); }
    static QString strEnglish() { return tr("English"); }
    static QString strNeder() { return tr("Dutch"); }
    static QString strRus() { return tr("Russian"); }

    static QString strNorskExampl;
    static QString strItalExampl;
    static QString strDeutschExampl;
    static QString strEnglishExampl;
    static QString strNederExampl;
		static QString strRusExampl;

    Tnote::EnameStyle getNameStyle();
		Tnote::EnameStyle getLetterStyle();
		Tnote::EnameStyle getSolfegeStyle();
		void setNameStyle(Tnote::EnameStyle style);

signals:
    void noteNameStyleWasChanged (Tnote::EnameStyle);

public slots:
    void noteNameStyleWasClicked();
    void seventhNoteWasChanged(bool isB);
		void styleButtonClicked();

private:
    QRadioButton *m_norskButt;
    QRadioButton *m_deutschButt;
    QRadioButton *m_italianoButt;
    QRadioButton *m_englishButt;
    QRadioButton *m_nederlButt;
		QRadioButton *m_rusButt;
		QRadioButton *m_letterRadio, *m_solfegeRadio;
    QButtonGroup *m_letterButtGroup, *m_solfegeButtGroup;

};

#endif // TNOTATIONRADIOGROUP_H
