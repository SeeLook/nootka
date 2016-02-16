/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#ifndef TGROUPEDQAUNIT_H
#define TGROUPEDQAUNIT_H

#include <QList>
#include <QString>
#include <QApplication>
#include <QDebug>

class TQAunit;


struct TqaPtr {
  TQAunit *qaPtr; // pointer to question in Texam
  unsigned int nr; // question number in global exam
};



/** 
 * This class describes question/answer unit (TQAunit) grouped by some features.
 */
class TgroupedQAunit
{
  
public:
  
  TgroupedQAunit();
  ~TgroupedQAunit();
  
  QList<TqaPtr> list;
  QString description() const { return m_desc; } /** Short description, usually for X axis tips. */
  void setDescription(const QString &desc) { m_desc = desc; }
  QString fullDescription() const { return m_fullDesc; }
  void addQAunit(TQAunit *qaUnit, unsigned int questNr); // appends TQAunit to the list
  void addQAunit(TqaPtr& qaPtr) { list << qaPtr; }
  
      /** Determines are wrong answers included to average
       * Default true - not included */
  static bool skipWrong() { return m_skipWrong; }
  static void setSkipWrong(bool skip) { m_skipWrong = skip; }
  TQAunit* first() { if (list.size()) 
                        return list[0].qaPtr;
                      else
                        return 0;
  } // Returns pointer to first element in the list or 0 when empty
  bool isEmpty() const { return list.isEmpty(); }
  int size() const { return list.size(); }
  
      /** Calculates mistakes and average time after appending all questions to the list.
       * Sets description. It doesn't add mistakes to average. 
       * If group contains only mistakes average time is calculated from mistakes time. 
       * @param desc - is short, single line description (html is accepted),
       * @param fullDesc - full description, multi lines accepted. */
  void resume(const QString& desc, const QString& fullDesc);
  qreal averTime() const { return m_averTime; }
  quint16 mistakes() const { return m_mistakes; }
  quint16 notBad() const { return m_halfMist; }
  qreal effectiveness() const { return m_effectiveness; }
  
  TqaPtr& operator[] (unsigned int index) { 
    if (index < list.size())
      return list[index];
    else { 
			qDebug() << "TgroupedQAunit operator[] would return pointer out of list scope! Last possible returned!";
			return list[list.size() - 1];
		}
  }
      /** Text: 
       *for a note: */
  static QString for_a_note()  { return QApplication::translate("chartStats", "for a note:", "average reaction time for..."); }
  
      /** Text: 
      *for a fret: */
  static QString for_a_fret()  { return QApplication::translate("chartStats", "for a fret:", "average reaction time for..."); }
  
      /** Text: 
      *for a key: */
  static QString for_a_key()  { return QApplication::translate("chartStats", "for a key:", "average reaction time for..."); }
  
      /** Text: 
      *for an accidental: */
  static QString for_an_accid()  { return QApplication::translate("chartStats", "for an accidental:", "average reaction time for..."); }
  
  
  
private:
  QString 				m_desc, m_fullDesc;
  quint16 				m_mistakes, m_halfMist;
  qreal 					m_averTime, m_effectiveness;
  static bool 		m_skipWrong;
    
};


#endif // TGROUPEDQAUNIT_H
