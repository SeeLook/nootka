/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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


#ifndef RANGESETTINGS_H
#define RANGESETTINGS_H

#include <QWidget>
#include "tlevel.h"

class TsimpleScore;
class QSpinBox;
class QCheckBox;

class rangeSettings : public QWidget
{
    Q_OBJECT
public:
    explicit rangeSettings(QWidget *parent = 0);

    void loadLevel(Tlevel level);
    void saveLevel(Tlevel &level);
		
		TsimpleScore* scoreRange() { return m_scoreRang; }

signals:
    void rangeChanged();
				/** Emitted when any string is checked or unchecked. 
				* Sends true when all string are selected, and false when not all. */
    void allStringsChecked(bool);

private:
    TsimpleScore *m_scoreRang;
    QSpinBox *m_fromSpinB, *m_toSpinB;
    QCheckBox *m_stringBut[6];

private slots:
    void stringSelected();
    void whenParamsChanged();
				/** When piano staff changes it enables third note. It keeps it disabled. */
		void whenPianoStaffChanges();

};

#endif // RANGESETTINGS_H
