/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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


#ifndef RANGESETTINGS_H
#define RANGESETTINGS_H


#include "tabstractlevelpage.h"

class QGroupBox;
class TlevelCreatorDlg;
class QPushButton;
class TsimpleScore;
class QSpinBox;
class QCheckBox;
class Tlevel;


class rangeSettings : public TabstractLevelPage
{
    Q_OBJECT
public:
    explicit rangeSettings(TlevelCreatorDlg* creator);

		virtual void loadLevel(Tlevel* level);
    virtual void saveLevel(Tlevel* level);
		
		TsimpleScore* scoreRange() { return m_scoreRang; }

signals:
    void rangeChanged();
		
				/** Emitted when any string is checked or unchecked. 
				* Sends true when all string are selected, and false when not all. */
    void allStringsChecked(bool);
		
public slots:
	virtual void changed();
	
protected:
    virtual void changedLocal();
		
private:
		QGroupBox 			*m_fretGr, *m_stringsGr;
    TsimpleScore 		*m_scoreRang;
    QSpinBox 				*m_fromSpinB, *m_toSpinB;
    QCheckBox 			*m_stringBut[6];
		QPushButton			*m_fretAdjustButt, *m_noteAdjustButt;
#if defined (Q_OS_ANDROID)
    QLabel          *m_fretAdjLabel, *m_noteAdjLabel, *m_strGrLabel;
#endif

private slots:
    void stringSelected();
    void whenParamsChanged();
		void adjustFrets();
		void adjustNotes();

};

#endif // RANGESETTINGS_H
