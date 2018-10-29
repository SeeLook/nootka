/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TYAXIS_H
#define TYAXIS_H


#include "tabstractaxis.h"


class QWidget;
class QStyleOptionGraphicsItem;


class TYaxis : public TabstractAxis
{

public:

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

  TYaxis();


      /**
       * Maximum value of a data on Y axis.
       * @p allowHalf determines when half of an unit vales can be displayed - it also depends on available space.
       */
  void setMaxValue(qreal val, bool allowHalf = true);
  qreal maxValue() { return m_maxVal; }
  double mapValue(double val) { return length() - TabstractAxis::mapValue(val); }

  virtual QRectF boundingRect() const;

  virtual QPainterPath shape() const;

      /**
       * sets an unit
       */
  void setUnit(Eunit unit);
  Eunit unit() {return m_unit; }

  void getYforGrid(QList<double> &yList); // Puts list of Y to yList coordinates to paint grid lines

      /**
       * Questions number
       */
  static QString questionsNumberTxt() { return QApplication::translate("TanalysDialog", "Questions number"); }

      /**
       * Attempts number
       */
  static QString attemptsNumberTxt() { return QApplication::translate("TanalysDialog", "Attempts number"); }

      /**
       * Preparation time
       */
  static QString prepareTimeTxt() { return QApplication::translate("TanalysDialog", "Preparation time"); }

      /**
       * Played number
       */
  static QString playedNumberTxt() { return QApplication::translate("TanalysDialog", "Played number"); }

protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);

private:
  qreal m_maxVal, m_multi, m_multi2;
  int m_textPosOffset; /**< half of text height */
  int m_loop, m_top;
  bool m_halfTick;
  Eunit m_unit;
  QString m_unitDesc; /**< unit description string f.e: time [s] */

};

#endif // TYAXIS_H
