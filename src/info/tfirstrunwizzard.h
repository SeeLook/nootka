/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                  				   *
 *   tomaszbojczuk@gmail.com   						                                 *
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


#ifndef TFIRSTRUNWIZZARD_H
#define TFIRSTRUNWIZZARD_H

#include <QDialog>

class QTextEdit;

class QStackedLayout;
class QRadioButton;
class QCheckBox;
class Tpage_3;
class Tpage_4;
class Select7note;
class TselectInstrument;

class TfirstRunWizzard : public QDialog
{
    Q_OBJECT
public:
    explicit TfirstRunWizzard(QWidget *parent = 0);
    static QString nextText() { return tr("next"); }

signals:

public slots:

private:
    QPushButton		 		*m_skipButt, *m_nextButt, *m_prevButt;
    QStackedLayout 		*m_pagesLay;
    Tpage_3 					*m_page3;
    Tpage_4 					*m_page4;
		QTextEdit 				*m_notationNote;
		TselectInstrument *m_selectInstr;

private slots:
    void nextSlot();
    void prevSlot();
		void whenInstrumentChanged(int instr);


};

// page 1 is about dialog
// This is page nr 1.5
class TselectInstrument : public QWidget
{
		Q_OBJECT
		
public:
		explicit TselectInstrument(QWidget* parent = 0);
		
		QRadioButton *classicalRadio, *electricRadio, *bassRadio, *otherRadio;
		
signals:
		void instrumentChanged(int);
		
private slots:
		void buttonPressed(int butt);
		
};

// page 2 is a QLabel with info about notation

class Tpage_3 : public QWidget
{
    Q_OBJECT
public:
    explicit Tpage_3(QWidget *parent = 0);
    
    static QString note7txt() { return tr("b", "Give here a name of 7-th note prefered in Your country. But only 'b' or 'h' not 'si' or something worst..."); } // returns b or h dependa on translation
				/** default style grab from translation 'letters' or 'solfege' */
    static QString keyNameStyle() { return tr("letters", "DO NOT TRANSLATE IT DIRECTLY. Put here 'letters' or 'solfege' This is country prefered style of nameing key signatures. 'letters' means C-major/a-minor names ('major' & 'minor' also are translated by You), 'solfege' means Do-major/La-minor names"); }

    Select7note *select7;
    QCheckBox *dblAccChB, *enharmChB, *useKeyChB;

};

class Tpage_4 : public QWidget
{
    Q_OBJECT
public:
  explicit Tpage_4(QWidget *parent = 0);
  
  
};

#endif // TFIRSTRUNWIZZARD_H
