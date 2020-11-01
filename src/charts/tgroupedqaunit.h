/***************************************************************************
 *   Copyright (C) 2013-2020 by Tomasz Bojczuk                             *
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

#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qcolor.h>


class TQAunit;


struct TqaPtr {
  TQAunit       *qaPtr; /**< pointer to question in Texam */
  quint16        nr; /**< question number in global exam */
  QColor         color;
  QPointF        pos;
  int            grNr = -1; /**< Number of group if question list is sorted to analysis */
};



class TtipInfo
{

public:

  enum Ekind : quint8 { e_point = 0, e_line = 1 };
  TqaPtr*   qaPtr() { return &p_qaPtr; }
  TQAunit*  qaUnit() { return p_qaPtr.qaPtr; }
  QColor    color() const { return p_qaPtr.color; }
  quint16   nr() const { return p_qaPtr.nr; }
  QPointF   cursorPos() const { return p_qaPtr.pos; }
  Ekind     kind = e_point;
  QString   tipText;
  int       grNr() { return p_qaPtr.grNr; }

  void setCursorPos(const QPointF& p) { p_qaPtr.pos = p; }
  void setQAUnit(TQAunit* u) { p_qaPtr.qaPtr = u; }
  void setNr(int n) { p_qaPtr.nr = n; }
  void setColor(const QColor& c) { p_qaPtr.color = c; }
  void setGr(int gn) { p_qaPtr.grNr = gn; }

protected:
  TqaPtr            p_qaPtr;
};

/**
 * This class describes question/answer unit (TQAunit) grouped by some features.
 */
class TgroupedQAunit
{

public:

  TgroupedQAunit() {}
  ~TgroupedQAunit();

  QList<TqaPtr> list;
  QString description() const { return m_desc; } /**< Short description, usually for X axis tips. */
  void setDescription(const QString &desc) { m_desc = desc; }
  QString fullDescription() const { return m_fullDesc; }
  void addQAunit(TQAunit *qaUnit, unsigned int questNr); /**< appends TQAunit to the list */
  void addQAunit(TqaPtr& qaPtr) { list << qaPtr; }

      /**
       * Determines are wrong answers included to average
       * Default true - not included
       */
  static bool skipWrong() { return m_skipWrong; }
  static void setSkipWrong(bool skip) { m_skipWrong = skip; }

      /**
       * Returns pointer to first element in the list or @p nullptr when empty
       */
  TQAunit* first() { return list.isEmpty() ? nullptr : list.first().qaPtr; }
  bool isEmpty() const { return list.isEmpty(); }
  int size() const { return list.size(); }

      /**
       * Calculates mistakes and average time after appending all questions to the list.
       * Sets description. It doesn't add mistakes to average.
       * If group contains only mistakes average time is calculated from mistakes time.
       * @p grNr - is number of group in the list
       * @p desc - is short, single line description (HTML is accepted),
       * @p fullDesc - full description, multi lines accepted.
       */
  void resume(const QString& desc, const QString& fullDesc);
  qreal averTime() const { return m_averTime; }
  quint16 mistakes() const { return m_mistakes; }
  quint16 notBad() const { return m_halfMist; }
  qreal effectiveness() const { return m_effectiveness; }

  TqaPtr& operator[] (unsigned int index) {
    if (index < list.size())
      return list[index];
    else {
      qDebug() << "[TgroupedQAunit] operator[] would return pointer out of list scope! Last possible returned!";
      return list.last();
    }
  }
      /**
       * Text:
       * for a note:
       */
  static QString for_a_note()  { return QGuiApplication::translate("chartStats", "for a note:", "average reaction time for..."); }

      /**
       * Text:
       * for a fret:
       */
  static QString for_a_fret()  { return QGuiApplication::translate("chartStats", "for a fret:", "average reaction time for..."); }

      /**
       * Text:
       * for a key:
       */
  static QString for_a_key()  { return QGuiApplication::translate("chartStats", "for a key:", "average reaction time for..."); }

      /**
       * Text:
       * for an accidental:
       */
  static QString for_an_accid()  { return QGuiApplication::translate("chartStats", "for an accidental:", "average reaction time for..."); }



private:
  QString         m_desc, m_fullDesc;
  quint16         m_mistakes = 0;
  quint16         m_halfMist = 0;
  qreal           m_averTime, m_effectiveness;
  static bool     m_skipWrong;

};


#endif // TGROUPEDQAUNIT_H
