/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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


#ifndef TCHART_H
#define TCHART_H

#include <QtQuick/qquickitem.h>
#include <QtGui/qguiapplication.h>


struct TqaPtr;
class TtipInfo;


/**
 * This is base class for charts in Nootka.
 */
class Tchart : public QObject
{

  Q_OBJECT

public:

  enum EanswersOrder {
    e_byNumber,
    e_byNote,
    e_byFret,
    e_byAccid,
    e_byKey,
    e_byMistake,
    e_byQuestAndAnsw, /**< both above */
    e_byStyle,
    e_byClef,
  };
  Q_ENUM(EanswersOrder)

      /**
       * Types of charts:
       * @p e_linear
       * @p e_bar
       * @p e_pie (not yet implemented)
       */
  enum EchartType {
    e_linear, e_bar, e_pie
  };

      /**
       * Kind of data represented by Y value of a point
       * @p e_questionTime - default and only one available for single note exams
       * @p e_effectiveness - Y axis displays effectiveness of single answer
       * @p e_prepareTime - Y axis displays time needed to start playing (only for played answers)
       * @p e_attemptsCount - Y axis displays number of attempts
       * @p e_playedCount - Y axis displays how many times melody was played
       * @p e_mistakesNumber - Y axis displays number of mistakes committed in single answer
       */
  enum EyValue {
    e_YquestionTime = 0, /**< - default and only one available for single note exams */
    e_Yeffectiveness,    /**< - Y axis displays effectiveness of single answer */
    e_YprepareTime,      /**< - Y axis displays time needed to start playing (only for played answers) */
    e_YattemptsCount,    /**< - Y axis displays number of attempts */
    e_YplayedCount,      /**< - Y axis displays how many times melody was played */
    e_YmistakesNumber    /**< - Y axis displays number of mistakes committed in single answer */
  };

  struct Tsettings {
    bool                inclWrongAnsw = false; /**< include wrong answers to average time of sorted group of answers */
    bool                separateWrong = true; /**< separate wrong answers and correct/almost good ones */
    EanswersOrder       order = e_byNumber;
    EchartType          type = e_linear;
    EyValue             yValue = e_YquestionTime;
  };

  /**
   * Unit of an axis values.
   */
  enum Eunit {
    e_timeInSec, /**< time in seconds (default) */
    e_questionNr, /**< number of questions */
    // melodies only:
    e_prepareTime, /**< time since question start to first played note */
    e_attemptsCount, /**< number of attempts */
    e_playedCount,
    e_effectiveness
  };

  explicit Tchart(QObject* parent = nullptr);

  virtual void setAnalyse(EanswersOrder order) { Q_UNUSED(order) }

  qreal maxValue() const { return m_maxValue; }
  void setYaxisParams(qreal maxV, Eunit yUnit);

  Eunit unit();
  void setUnit(Eunit unit);

      /**
       * Questions number
       */
  static QString questionsNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Questions number"); }
  
      /**
       * Attempts number
       */
  static QString attemptsNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Attempts number"); }
  
      /**
       * Preparation time
       */
  static QString prepareTimeTxt() { return QGuiApplication::translate("AnalyzeDialog", "Preparation time"); }
  
      /**
       * Played number
       */
  static QString playedNumberTxt() { return QGuiApplication::translate("AnalyzeDialog", "Played number"); }

  QString yAxisLabel() const { return m_unitDesc; }

      /**
       * Exactly number of items on X axis, usually equivalent of number of questions on linear chart
       * but also number of bars on bar chart
       */
  int xCount() { return p_xCount; }

  QList<qreal> yTickList() const { return m_yTickList; }

      /**
       * @p p_averChunks is a list of average reaction times for every next question.
       * Means, for first quest it is reaction time for second it is average of them both and so on
       * When wrong answers are excluded from average corresponding value is the same as latest average of correct/not bad answer
       * @p averChunk() method gives access to those partial average values
       */
  qreal averChunk(int qNr) { return qNr > -1 && qNr < p_averChunks.size() ? p_averChunks[qNr] : 0.0; }

signals:
  void hoveredChanged();

protected:
  int                 p_xCount = 0;
  QList<qreal>        p_averChunks;

private:
  qreal               m_maxValue;
  Eunit               m_unit;
  QString             m_unitDesc; /**< unit description string f.e: time [s] */
  QList<qreal>        m_yTickList;

};
#endif // TCHART_H
