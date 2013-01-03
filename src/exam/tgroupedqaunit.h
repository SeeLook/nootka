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


#ifndef TGROUPEDQAUNIT_H
#define TGROUPEDQAUNIT_H

#include <QList>
#include <QString>

class TQAunit;


struct TqaPtr {
  TQAunit *qaPtr; // pointer to question in Texam
  unsigned int nr; // question number in global exam
};



/** This class sescribes question/asnwer unit (TQAunit) grouped by some features.
 */
class TgroupedQAunit
{
  
public:
  
  TgroupedQAunit();
  ~TgroupedQAunit();
  
  QList<TqaPtr> list;
  QString description() { return m_desc; }
  void setDescription(QString &desc) { m_desc = desc; }
  void addQAunit(TQAunit *qaUnit, unsigned int questNr); // appends TQAunit to the list
  void addQAunit(TqaPtr& qaPtr) { list << qaPtr; }
  TQAunit* first() { if (list.size()) 
                        return list[0].qaPtr;
                      else
                        return 0;
  } // Returns pointer to first element in the list or 0 when empty
  bool isEmpty() { return list.isEmpty(); }
  int size() { return list.size() ;}
  
  void resume(); // calculates mistakes and average time after appending all questions to the list
  
  TqaPtr& operator[] (unsigned int index) { 
    if (index < list.size())
      return list[index];
//     else return 0;
  }
  
  
private:
  QString m_desc;
  quint16 m_mistakes, m_halfMist;
  
  
};


#endif // TGROUPEDQAUNIT_H
