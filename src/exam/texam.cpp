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
#include "tqaunit.h"
#include "tlevelselector.h"
#include <QFile>
#include <QDataStream>


/*static*/
const qint32 Texam::examVersion = 0x95121702;


Texam::Texam():
	m_level(0)	
{

}

Texam::Texam(Tlevel* l, QString userName):
	m_level(l),
	m_userName(userName)
{

}


Texam::~Texam()
{
  delete m_level;
}


Texam::EerrorType Texam::loadExamFromFile(QString& fileName) {
	QFile file(fileName);
//     quint16 questNr, mistNr, tmpAverTime, averTime = 0;
	quint16 questNr;
	m_workTime = 0;
	EerrorType result = e_file_OK;
    if (file.open(QIODevice::ReadOnly)) {
	     QDataStream in(&file);
         in.setVersion(QDataStream::Qt_4_7);
         quint32 ev; //exam template version
         in >> ev;
         if (ev != examVersion)
             return e_file_not_valid;
		 
         in >> m_userName;
         getLevelFromStream(in, *(m_level);
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
                 m_workTime += qaUnit.time; // 
                 if ( !qaUnit.correct() )
                     tmpMist++;
//                     qDebug() << isExamFileOk << m_answList.size() <<
//                             (int)qaUnit.key.value() << qaUnit.key.isMinor();
                 }
                 if (tmpMist != m_mistNr || questNr != m_answList.size()) {
                   isExamFileOk = false;
                   m_mistNr = tmpMist; //we try to fix exam file to give proper number of mistakes
                 }
                 if (!isExamFileOk)
                     result = e_file_corrupted;        
     } else {
         TlevelSelector::fileIOerrorMsg(file, 0);
		 result = e_cant_open;
     }
     return result;
}


Texam::EerrorType Texam::saveExamToFile(QString& fileName) {

}


