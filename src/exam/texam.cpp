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


#include "texam.h"
#include "texamlevel.h"
#include "tlevelselector.h"
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QDebug>

/*static*/
const qint32 Texam::examVersion = 0x95121702;



Texam::Texam(TexamLevel* l, QString userName):
	m_level(l),
	m_userName(userName),
	m_fileName(""),
	m_mistNr(0),
	m_workTime(0)
{

}


Texam::~Texam()
{}


Texam::EerrorType Texam::loadFromFile(QString& fileName) {
    m_fileName = fileName;
    QFile file(fileName);
    quint16 questNr;
    m_workTime = 0;
    m_mistNr = 0;
    EerrorType result = e_file_OK;
    if (file.open(QIODevice::ReadOnly)) {
      QDataStream in(&file);
      in.setVersion(QDataStream::Qt_4_7);
      quint32 ev; //exam template version
      in >> ev;
      if (ev != examVersion)
          return e_file_not_valid;

      in >> m_userName;
      getLevelFromStream(in, *(m_level));
      in >> m_tune;
      in >> m_totalTime;
      in >> questNr >> m_averReactTime >> m_mistNr;
      bool isExamFileOk = true;
      int tmpMist = 0;
      while (!in.atEnd()) {
          TQAunit qaUnit;
          if (!getTQAunitFromStream(in, qaUnit))
              isExamFileOk = false;
          m_answList << qaUnit;
//           qDebug() << m_answList.size() << QString::fromStdString(qaUnit.qa.note.getName());
          m_workTime += qaUnit.time; // 
          if ( !qaUnit.correct() )
              tmpMist++;
//                     qDebug() << isExamFileOk << m_answList.size() <<
//                             (int)qaUnit.key.value() << qaUnit.key.isMinor();
          }
          if (tmpMist != m_mistNr || questNr != m_answList.size()) {
            isExamFileOk = false;
//             qDebug() << "mistakes:" << tmpMist << m_mistNr << "questions:" << questNr << m_answList.size();
            m_mistNr = tmpMist; //we try to fix exam file to give proper number of mistakes
          }
          m_averReactTime = m_workTime / count();
//           m_workTime = qRound((qreal)m_workTime / 10.0);
          if (!isExamFileOk)
              result = e_file_corrupted;        
     } else {
         TlevelSelector::fileIOerrorMsg(file, 0);
				 result = e_cant_open;
     }
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
		out << examVersion;
		out << m_userName << *m_level << m_tune;
		out << m_totalTime; // elapsed exam time (quint32)
			// data for file preview
		out << (quint16)m_answList.size(); // number of questions
		out << m_averReactTime; // average time of answer (quint16)
			// that's all
		out << m_mistNr; // number of mistakes (quint16)
		for (int i = 0; i < m_answList.size(); i++)
				out << m_answList[i]; // and obviously answers
	} else {
		QMessageBox::critical(0, "",
           QObject::tr("Cannot save exam file:\n%1").arg(QString::fromLocal8Bit(qPrintable(file.errorString()))));
		return e_cant_open;
	}
	return e_file_OK;
}

void Texam::setAnswer(TQAunit& answer) {
    m_answList.last() = answer;
    if (!answer.correct())
      m_mistNr++;
    m_workTime += answer.time;
}

