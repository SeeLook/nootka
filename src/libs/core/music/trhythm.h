/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TRHYTHM_H
#define TRHYTHM_H

#include <QString>
#include <cmath>


const std::string rhythmStrings [6] = {"", "whole", "half", "quarter", "eighth", "16th"};


/** 
 * This class describes musical note value (relative duration)
 */
class Trhythm
{

public:
	
	enum EnoteValue {
		e_none = 0, e_whole = 1, e_half = 2, e_quarter = 4, e_eighth = 8, e_sixteenth = 16
	}; /** Describes note duration */
	
			/** Creates note value, by default it is quarter (without dot) */
	Trhythm(EnoteValue nVal = e_quarter, bool rest = false, bool dot = false) :
		m_noteVal(nVal), m_isRest(rest), m_hasDot(dot)
	{}
	
			/** Sets rhythm parameters */
	void setRhythm(EnoteValue nVal, bool rest = false, bool dot = false) {
		m_noteVal = nVal;
		m_isRest = rest;
		m_hasDot = dot;
	}
	
	EnoteValue rhythm() { return m_noteVal; }
	void setNoteValue(EnoteValue nVal) { m_noteVal = nVal; }
	
			/** It converts QString into rhythm value */
	void setNoteValue(const std::string& nVal) {
		for (int i = 0; i < 6; ++i) {
			if (nVal == rhythmStrings[i]) {
				m_noteVal = (EnoteValue)i;
				return;
			}
		}
	}
	
	bool hasDot() { return m_hasDot; }
	void setDot(bool dot) { m_hasDot = dot; }
	
	bool isRest() { return m_isRest; }
	void setRest(bool rest) { m_isRest = rest; }
	
	QString xmlType() {
		for (int i = 1; i < 6; ++i) {
			if (pow(2, i - 1) == (int)m_noteVal)
				return QString::fromStdString(rhythmStrings[i]);
		}
		return "";
	}
	
	
private:
	EnoteValue 							m_noteVal;
	bool 										m_hasDot, m_isRest;
};

#endif // TRHYTHM_H
