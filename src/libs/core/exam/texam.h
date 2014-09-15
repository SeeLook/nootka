/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#ifndef TEXAM_H
#define TEXAM_H


#include <nootkacoreglobal.h>
#include <QString>
#include <QList>
#include <QXmlStreamWriter>
#include <QApplication>
#include "tqaunit.h"
#include <music/ttune.h>

class Tlevel;


/** 
* This class describes instance of an exam.  
*/
class NOOTKACORE_EXPORT Texam
{

public:
      /** An exam constructor.
       * @param Tlevel has to be pointer to existing exam level       
       */
    explicit Texam(Tlevel *l, QString userName);
    virtual ~Texam();
		
			/** Magic numbers in exam file to identify it.*/
		static const qint32 examVersion;
		static const qint32 examVersion2;
		static const qint32 currentVersion;
		
		static bool isExamVersion(qint32 ver); /** Returns true when given value match to exam versions. */
		
				/** Returns number of exam version like 1, 2 or so or -1 when given version is not valid. */
		static int examVersionNr(qint32 ver);
		
				/** Returns true when given value 'could be' or 'it is' some version of exam file.
				 * This way exams created with newer Nootka version can be detected. */
		static bool couldBeExam(qint32 ver);
		
				/** Generates exam version identifier number from given simple version number (1, 2 etc.)
				 * It doesn't verify does given number make sense! */
		static qint32 getVersionId(quint8 verNr) { return examVersion + (verNr - 1) * 2; }
		
				/** Returns level version number used in given exam version
				 * It doesn't check is given exam version valid!. */
		static qint32 examVersionToLevel(qint32 examVer);
		
				/** returns nice formatted time (1:05:15.3). Time is in seconds multiplied by 10.
        * When withUnit is true adds s (seconds) unit*/
    static QString formatReactTime(quint16 timeX10, bool withUnit = false);
		
				/** Possible errors during opening and saving exam file.*/
    enum EerrorType { e_file_OK = 0,
                    e_file_not_valid, // occurs when examVersion is different
                    e_file_corrupted, // when data in file is corrupted
                    e_cant_open, // problems with reading file	
                    e_noFileName,
										e_newerVersion // when file version is newer than this Nootka version can support
          };

  Tlevel* level() { return m_level; }
  void setLevel(Tlevel *l) { m_level = l; }
  Ttune tune() { return m_tune; }
  void setTune(Ttune &tune) { m_tune = tune; }

  quint32 totalTime() { return m_totalTime; }
  void setTotalTime(quint32 total) { m_totalTime = total; }

			/** Adds @param TQAunit object at the end of the questions list. */
  void addQuestion(TQAunit &question) { m_answList << question; }
  
			/** Overwrites the last element on the questions list. */
  void setAnswer(TQAunit &answer);
	
			/** Removes last question from the list and sets times according to it*/
	void removeLastQuestion();
	TQAunit& curQ() { return m_answList.last(); } /** Last q/a unit */
  QList<TQAunit>* answList() { return &m_answList; }
  
			/** Returns number of questions/answers in en exam. */
  int count() { return m_answList.size(); }
  
			/** Returns number of committed mistakes in en exam. */
  quint16 mistakes() { return m_mistNr; }
  quint16 halfMistaken() { return m_halfMistNr; }
  quint16 averageReactonTime() { return m_averReactTime; }
  void setAverageReactonTime(quint16 avTime) { m_averReactTime = avTime; }
  
			/** Total time spent for answering without breaks between questions */
  quint16 workTime() { return qRound((qreal)m_workTime / 10.0); }
  QString userName() { return m_userName; }
  QString fileName() { return m_fileName; }
  void setFileName(QString fileName) { m_fileName = fileName; }
 
			/** Reference to list of mistakes. */
  QList<TQAunit>* blacList() { return &m_blackList; }
  int penalty() { return m_penaltysNr; } // Number of penalties during whole exam
  int blackCount() { return m_blackCount; } // Remained questions in black list
  bool isFinished() { return m_isFinished; }
  
			/** Sets exam as finished and there is no way back. */
  void setFinished() { m_isFinished = true; }
  void increasePenaltys(int penaltyNr) { m_penaltysNr += penaltyNr; }
  
      /** Effectiveness of an exam.  */
  qreal effectiveness() { return effectiveness(count(), m_mistNr, m_halfMistNr); }

  EerrorType loadFromFile(QString &fileName);
  EerrorType saveToFile(QString fileName = "");
	
	void writeToXml(QXmlStreamWriter& xml);
	bool loadFromXml(QXmlStreamReader& xml);
	bool loadFromBin(QDataStream& in, quint32 ev);

			/** Maximal time of an answer = 65500. Values over are equal to it.
			* 65501 & 65502 are for counting probes in blackList */
  static const quint16 maxAnswerTime;
	
			/** Compares given questions are they the same. */
  static bool areQuestTheSame(TQAunit &q1, TQAunit &q2);
	
			/** Static method  calculates effectiveness from given values
			* (((qreal)questNumber - (qreal)(mistakes + (notBad / 2))) / (qreal)questNumber) * 100 */
  static qreal effectiveness(int questNumber, int mistakes, int notBad);
	
			/** Returns a reference to question/answer unit nr @param index.
			* Be Aware !!! index has to be less than m_answList.size() */
  TQAunit &question(unsigned int index) { return m_answList[index]; }

protected:
    /** Iterates through m_blackList to calculate number */
  void updateBlackCount();
	
    /** Grabs answers with mistakes and creates black list */
  void convertToVersion2();
	
		/** Reads TQAunit from given XML and appends it at the end of given list or prints error and return @p FALSE*/
	bool readUnitFromXml(QList<TQAunit>& list, QXmlStreamReader& xml);
	
	void grabFromLastUnit(); /** increases some counter variables to obtain checking values */
  

private:
	QString 									m_fileName, m_userName;
	Tlevel 									 *m_level;
	QList<TQAunit> 						m_answList;
  QList<TQAunit> 						m_blackList;
	Ttune 										m_tune;
	quint32 									m_totalTime;
	quint16 									m_mistNr, m_averReactTime, m_workTime, m_halfMistNr;
  bool 											m_isFinished;
  int 											m_penaltysNr;
  int 											m_blackCount;

  
};

#endif // TEXAM_H
