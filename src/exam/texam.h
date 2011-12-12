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
#include "tqaunit.h"
#include "ttune.h"

class TexamLevel;


/** This class describes instance of an exam.  */
class Texam
{

public:
      /** An exam constructor.
       * @param TexamLevel has to be pointer to existing exam level       
       */
    explicit Texam(TexamLevel *l, QString userName);
    virtual ~Texam();

      /** Possible errors during opening and saveing exam file.*/
    enum EerrorType { e_file_OK = 0,
                    e_file_not_valid, // occurs when examVersion is different
                    e_file_corrupted, // when data in file is corrupted
                    e_cant_open, // problems with reading file	
                    e_noFileName
          };

  TexamLevel* level() { return m_level; }
  void setLevel(TexamLevel *l) { m_level = l; }
  Ttune tune() { return m_tune; }
  void setTune(Ttune tune) { m_tune = tune; }

  quint32 totalTime() { return m_totalTime; }
  void setTotalTime(quint32 total) { m_totalTime = total; }

    /** Adds @param TQAunit object at the end of the questions list. */
  void addQuestion(TQAunit &question) { m_answList << question; }
    /** Overvrites the last element on the questions list. */
  void setAnswer(TQAunit &answer); 
  TQAunit curQ() { return m_answList.last(); }
  QList<TQAunit>* answList() { return &m_answList; }
    /** Returns number of questions/answers in en exam. */
  int count() { return m_answList.size(); }
    /** Returns number of commited mistakes in en exam. */
  quint16 mistakes() { return m_mistNr; }
  quint16 averageReactonTime() { return m_averReactTime; }
  void setAverageReactonTime(quint16 avTime) { m_averReactTime = avTime; }
    /** Total time spent for answering without breaks between questions */
  quint16 workTime() { return m_workTime; }
  QString userName() { return m_userName; }
  QString fileName() { return m_fileName; }
  void setFileName(QString fileName) { m_fileName = fileName; }


  EerrorType loadFromFile(QString &fileName);
  EerrorType saveToFile(QString fileName = "");

    /** Magic number in exam file to identify it.*/
  static const qint32 examVersion;


private:
	QString m_fileName, m_userName;
	TexamLevel *m_level;
	QList<TQAunit> m_answList;
	Ttune m_tune;
	quint32 m_totalTime;
	quint16 m_mistNr, m_averReactTime, m_workTime;
		
		
		
};

#endif // TEXAM_H
