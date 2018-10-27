/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TXAXIS_H
#define TXAXIS_H


#include "tabstractaxis.h"


class TgroupedQAunit;
class Tlevel;
class TQAunit;


/**
 * This is X axis of charts. It represents questions.
 * Its size is (length) is calculated automatically by questions number.
 */
class TXaxis : public TabstractAxis
{

public:
  TXaxis(QList<TQAunit*>* answers = 0, Tlevel* level = 0);

  void setAnswersList(QList<TQAunit*>* answers, Tlevel* level = 0);
  void setAnswersLists(QList<TgroupedQAunit> &listOfLists, Tlevel *level = 0);
  void setAnswersForBarChart(QList<TgroupedQAunit> &listOfLists);

      /**
       * pixel width of question on the axis
       */
  int questWidth() { return m_qWidth; }

  virtual QRectF boundingRect();
  
protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  void setTicText(QGraphicsTextItem* tic, TQAunit* unit, int questNr = 0);

private:
  const int                     m_qWidth; /**< pixel width of question on the axis */
  QList<TQAunit*>              *m_answers;
  Tlevel                       *m_level;
  QList<QGraphicsTextItem*>     m_ticTips;
};

#endif // TXAXIS_H
