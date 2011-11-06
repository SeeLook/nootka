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

#ifndef TEXAM_H
#define TEXAM_H
#include <QString>
#include <QList>

class Ttune;
class TQAunit;
class Tlevel;



/** This class describes instance of an exam.
 * 
 */
class Texam
{

public:
    explicit Texam();
	Texam(Tlevel *l);
	Texam(QString examFile);
    virtual ~Texam();
	
	Tlevel* level() { return m_level; }
	void setLevel(Tlevel *l);
	
	void addQuestion(TQAunit &question) { m_answList << question; }
	void setAnswer(TQAunit &answer) { m_answList.last() = answer; }
	TQAunit curQ() { return m_answList.last(); }
	
	
	bool loadExamFromFile(QString &fileName);
	bool saveExamToFile(QString &fileName = 0);
	
	  /** Magic number in exam file to identify it.*/
	static const qint32 examVersion;
	
	
private:
	QString m_fileName, m_userName;
	Tlevel *m_level;
	QList<TQAunit> m_answList;
	Ttune m_tune;		
		
		
		
};

#endif // TEXAM_H
