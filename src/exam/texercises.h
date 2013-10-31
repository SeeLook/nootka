/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#ifndef TEXERCISES_H
#define TEXERCISES_H

#include <QDialog>

class QRadioButton;
class Texam;

/** Class for managing exercising (exam like mode) */
class Texercises
{

public:
		Texercises(Texam *exam);
		
		void checkAnswer();
		
				/** Sets checking of exercising progress enabled when @p obligate value is bigger than 0.
				 * Given value should be obligQuestions() calculated by @class TexecutorSupply  */
		void setSuggestionEnabled(int obligateNr);
		
				/** Returns user decision when he got message about starting an exam.
				 * It affects global setting for further suggestions. */
		bool suggestInFuture() { return m_chekInFuture; }
		
				/** Returns true when user decided to start exam after suggestion.  */
		bool readyToExam() { return m_readyToExam; }
		
private:
		Texam				*m_exam;
		bool 				m_chekInFuture, m_checkNow, m_readyToExam;
		int 				m_max, m_currentGood;

};



class TsuggestExam : public QDialog
{
	Q_OBJECT
public:
		explicit TsuggestExam();
		
		enum Esuggest {
			e_readyToExam, e_forAmoment, e_notThisTime, e_neverEver
		};
		
				/** Call this dialog and returns user decision */
		Esuggest suggest();
		
		
private:
		QRadioButton	*m_redyExamRadio, *m_notNowRadio, *m_notThisExRadio, *m_neverAskRadio;
		Esuggest			m_userResponse;
		
};

#endif // TEXERCISES_H

