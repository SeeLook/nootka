/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TFIXLEVELWIDGET_H
#define TFIXLEVELWIDGET_H


#include "nootkacoreglobal.h"
#include <QDialog>


class Tlevel;

			/** This function checks given level is it necessary to fix an instrument in it,
			 * if user set auto fix - it fixes the level silently or displays a dialog window for user interaction. 
			 * Then stores user decision back to @p autoFixInstr
			 * Parameters are:
			 * @p level - level to fix
			 * @p fileName 	- name of the exam or level file (can be empty - "")
			 * @p autoFixInstr - reference to global parameter storing user preference of fixing instrument.
			 * It can be -1 (user interaction to fix) or 
			 * Einstrument cast to int value with user preferred instrument.
			 * @p prefInstr - current user preferred instrument in Nootka
			 */
NOOTKACORE_EXPORT bool fixLevelInstrument(Tlevel& level, const QString& fileName, int& autoFixInstr, QWidget* parent = 0);


//*********************************************************************************************************

class QCheckBox;
class TselectInstrument;


class NOOTKACORE_EXPORT TfixLevelDialog : public QDialog
{
    Q_OBJECT

public:
			/** Creates a dialog window to fix instrument. Parameters are:
			 * @p fileName 	- name of the exam or level file 
			 * @p autoFix 	- reference to global variable that fixing should be automatic
			 * @p prefInstr - preferred instrument and reference through which user selected instrument is returned
			 */
	explicit TfixLevelDialog(Tlevel& level, const QString& fileName, bool& autoFix, int& prefInstr, QWidget* parent = 0);
	
protected slots:
	void okSlot();

private:
	TselectInstrument 		*m_selInstr;
	QCheckBox							*m_autoCheckBox;
	bool 									&m_autoFix;
	int										&m_instr;	
};

#endif // TFIXLEVELWIDGET_H
