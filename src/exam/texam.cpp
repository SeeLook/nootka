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


#include "texam.h"
#include "texamlevel.h"
#include "tlevelselector.h"
#include "tglobals.h"
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>


extern Tglobals *gl;

/*static*/
const qint32 Texam::examVersion = 0x95121702;
const qint32 Texam::examVersion2 = 0x95121704;

const quint16 Texam::maxAnswerTime = 65500;

bool Texam::areQuestTheSame(TQAunit& q1, TQAunit& q2) {
  if (q1.questionAs == q2.questionAs && // the same questions
      q1.answerAs == q2.answerAs && // the same answers
      q1.qa.note == q2.qa.note && // ths same notes
      q1.qa.pos == q2.qa.pos // ths same pos
    )
        return true;
  else
        return false;
}

qreal Texam::effectiveness(int questNumber, int mistakes, int notBad) {
    return (((qreal)questNumber - (qreal)(mistakes + ((qreal)notBad / 2))) / (qreal)questNumber) * 100.0;   
}
/*end of static*/


Texam::Texam(TexamLevel* l, QString userName):
  m_level(l),
  m_userName(userName),
  m_fileName(""),
  m_mistNr(0),
  m_workTime(0),
  m_penaltysNr(0),
  m_isFinished(false),
  m_halfMistNr(0),
  m_blackCount(0)
{

}


Texam::~Texam()
{
  m_answList.clear();
  m_blackList.clear();  
}


Texam::EerrorType Texam::loadFromFile(QString& fileName) {
    m_fileName = fileName;
    QFile file(fileName);
    quint16 questNr;
    m_workTime = 0;
    m_mistNr = 0;
    m_blackCount = 0;
    m_blackList.clear();
    m_answList.clear();
    EerrorType result = e_file_OK;
    quint32 ev; //exam template version
    if (file.open(QIODevice::ReadOnly)) {
      QDataStream in(&file);
      in.setVersion(QDataStream::Qt_4_7);
      in >> ev;
      if (ev != examVersion && ev != examVersion2)
          return e_file_not_valid;

      in >> m_userName;
      getLevelFromStream(in, *(m_level));
      in >> m_tune;
      in >> m_totalTime;
      in >> questNr >> m_averReactTime >> m_mistNr;
      if (ev == examVersion2) {
        in >> m_halfMistNr >> m_penaltysNr >> m_isFinished;
      } else {
        m_halfMistNr = 0;
        m_penaltysNr = 0;
        m_isFinished = false;
      }
      bool isExamFileOk = true;
      int tmpMist = 0;
      int tmpHalf = 0;
      int fixedNr = 0;
      int okTime = 0; // time of correct and notBad answers to calculate average
      while (!in.atEnd()) {
          TQAunit qaUnit;
          if (!getTQAunitFromStream(in, qaUnit))
              isExamFileOk = false;
          if ((qaUnit.questionAs == TQAtype::e_asName || qaUnit.answerAs == TQAtype::e_asName) 
                && qaUnit.styleOfQuestion() < 0) {
                  qaUnit.setStyle(gl->NnameStyleInNoteName, qaUnit.styleOfAnswer());
                  fixedNr++;
              } /** In old versions style was set to 0 so now it gives styleOfQuestion = -1
                * Also in transistional Nootka versios it was left unchanged.
                * Unfixed it involes stiupid namse in charts.
                * We are fixing it by insert user prefered style of nameing */
          if (qaUnit.time <= maxAnswerTime || ev == examVersion) { // add to m_answList
              m_answList << qaUnit;
              m_workTime += qaUnit.time;
              if ( !qaUnit.isCorrect() ) {
                if (qaUnit.isWrong())
                  tmpMist++;
                else
                  tmpHalf++; // not so bad answer
              }
              if (!qaUnit.isWrong())
                  okTime += qaUnit.time;
          } else { // add to m_blackList
              m_blackList << qaUnit;
          }
      }
      if (questNr != m_answList.size()) {
        isExamFileOk = false;        
      }
      if (ev == examVersion2 && (tmpMist != m_mistNr || tmpHalf != m_halfMistNr)) {
        m_mistNr = tmpMist; //we try to fix exam file to give proper number of mistakes
        m_halfMistNr = tmpHalf;
        isExamFileOk = false;
      } else {
        m_mistNr = tmpMist; // transistion to examVersion2
      }
      if (ev == examVersion) {
          convertToVersion2();
          m_halfMistNr = tmpHalf;
      }
      if (fixedNr)
          qDebug() << "fixed style in questions:" << fixedNr;
//       m_averReactTime = m_workTime / count(); // OBSOLETE
      if ((count() - mistakes()))
        m_averReactTime = okTime / (count() - mistakes());
      else 
        m_averReactTime = 0.0;
      if (!isExamFileOk)
          result = e_file_corrupted;        
     } else {
         TlevelSelector::fileIOerrorMsg(file, 0);
          result = e_cant_open;
     }
  updateBlackCount();
  qDebug() << "black questions:" << blackCount() << "Mistakes:" << m_mistNr << "Not so bad:" << m_halfMistNr;
  return result;
}


Texam::EerrorType Texam::saveToFile(QString fileName) {
	if (fileName != "")
		setFileName(fileName); // m_fileName becomes fileName
	if (m_fileName == "")
		return e_noFileName;
	QFile file(m_fileName);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_7);
		out << examVersion2;
		out << m_userName << *m_level << m_tune;
		out << m_totalTime; // elapsed exam time (quint32)
			// data for file preview
		out << (quint16)m_answList.size(); // number of questions
		out << m_averReactTime; // average time of answer (quint16)
			// that's all
		out << m_mistNr; // number of mistakes (quint16)
      /** Those were added in examVersion2 */
		out << m_halfMistNr << m_penaltysNr << m_isFinished;
		for (int i = 0; i < m_answList.size(); i++)
				out << m_answList[i]; // and obviously answers
	  if (m_blackList.size()) {
      for (int i = 0; i < m_blackList.size(); i++)
        out << m_blackList[i]; // and black list
    }
	} else {
		QMessageBox::critical(0, "",
           QObject::tr("Cannot save exam file:\n%1").arg(QString::fromLocal8Bit(qPrintable(file.errorString()))));
		return e_cant_open;
	}
	return e_file_OK;
}

void Texam::setAnswer(TQAunit& answer) {
    answer.time = qMin(maxAnswerTime, answer.time); // when user think too much
    m_answList.last() = answer;
    if (!answer.isCorrect()) {
      if (!isFinished()) // finished exam has got no black list
          m_blackList << answer;
      if (answer.isNotSoBad()) {
        if (!isFinished()) {
            m_blackList.last().time = 65501;
            m_penaltysNr++;
        }
        m_halfMistNr++;
      }
      else {
        if (!isFinished()) {
            m_blackList.last().time = 65502;
            m_penaltysNr += 2;
        }
        m_mistNr++;
      }
    }
    m_workTime += answer.time;
    if (!isFinished())
        updateBlackCount();
}



//############################### PROTECTED ########################################



void Texam::updateBlackCount() {
  m_blackCount = 0;
  if (m_blackList.size()) {
    for (int i = 0; i < m_blackList.size(); i++)
    m_blackCount += (m_blackList[i].time - maxAnswerTime);
  }
}


void Texam::convertToVersion2() {
  bool hasStyle = false;
  Tnote::EnameStyle randStyles[3];
  if (m_level->canBeName()) {
    // ver. 1 didn't put proper Tnote::EnameStyle to file - we fixing it
    hasStyle = true;
    qDebug("Fixing styles of note names in file");
    qsrand(QDateTime::currentDateTime().toTime_t());
   if (m_level->requireStyle) { // prepare styles array to imitate switching
      randStyles[0] = Tnote::e_italiano_Si;
      if (gl->seventhIs_B) {
        randStyles[1] = Tnote::e_english_Bb;
        randStyles[2] = Tnote::e_nederl_Bis;
      } else {
        randStyles[1] = Tnote::e_norsk_Hb;
        randStyles[2] = Tnote::e_deutsch_His;
      }
   }
  }
  
  for (int i = 0; i < m_answList.size(); i++) {
    if (m_answList[i].time > maxAnswerTime) // fix too long times in ver1 if any
        m_answList[i].time = maxAnswerTime;
  // ver. 1 didn't put proper Tnote::EnameStyle to file - we fixing it
    if (hasStyle) {
      if (m_level->requireStyle) {
        if (m_answList[i].questionAs == TQAtype::e_asName && m_answList[i].answerAs == TQAtype::e_asName) {
          Tnote::EnameStyle qSt = randStyles[qrand() % 3];
          Tnote::EnameStyle aSt;
          if (qSt == Tnote::e_italiano_Si)
            aSt = randStyles[(qrand() % 2) +1];
          else
            aSt = Tnote::e_italiano_Si;
          m_answList[i].setStyle(qSt, aSt);
        } else
          if (m_answList[i].questionAs == TQAtype::e_asName) {
            m_answList[i].setStyle(randStyles[qrand() % 3], gl->NnameStyleInNoteName);
          } else
            if (m_answList[i].questionAs == TQAtype::e_asName) {
              m_answList[i].setStyle(gl->NnameStyleInNoteName, randStyles[qrand() % 3]);
            }
      } else // fixed style - we changeing to user prefered
          m_answList[i].setStyle(gl->NnameStyleInNoteName, gl->NnameStyleInNoteName);
    }
      
    if (!m_answList[i].isCorrect()) {
      quint16 penCnt = 0; // counts of penaltys
      if (m_answList[i].isWrong()) {
        if (i < (m_answList.size() -1) && areQuestTheSame(m_answList[i], m_answList[i+1])) {
          // there was next question repeated
          if (m_answList[i+1].isCorrect()) // and was correct
            penCnt = 65501; // so add one penalty
          else // when again wrong
            penCnt = 65502; // add two
          // The next loop will add next two penaltys !!
        } else // question was not repeated
            penCnt = 65502;
      } else { // not so bad
        if (i < (m_answList.size() -1) && areQuestTheSame(m_answList[i], m_answList[i+1])) {
          // there was next question repeated
          if (m_answList[i+1].isCorrect()) // and was correct
//             m_blackList.removeLast(); // remove it from black list - corrected
            penCnt = 0;
          else
            penCnt = 65501;
        }
      }
      if (penCnt) {
        m_blackList << m_answList[i];
        m_blackList.last().time = penCnt;
        m_penaltysNr += (penCnt - 65500);
      }
    }
  }
  qDebug() << "Converted to exam version 2!!!  black list:" << m_blackList.size() << "penaltys:" << m_penaltysNr;
}



