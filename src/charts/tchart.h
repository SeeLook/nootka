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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TCHART_H
#define TCHART_H

#include <QtQuick/qquickpainteditem.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtGui/qpainter.h>
#include "tmainline.h"


class TYaxis;
class TXaxis;
class TqaPtr;
class TtipInfo;


/**
 * This is base class for charts in Nootka.
 * It has got @p QGraphicsScene *scene() and
 * two axis-es xAxis and yAxis which are created by default.
 */
class Tchart : public QQuickPaintedItem
{

  Q_OBJECT

  friend class TmainLine;

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

      /**
       * Types of charts.
       */
  enum EchartType {
    e_linear, e_bar, e_pie
  };

  struct Tsettings {
    bool                inclWrongAnsw = false; /**< include wrong answers to average time of sorted group of answers */
    bool                separateWrong = true; /**< separate wrong answers and correct/almost good ones */
    EanswersOrder       order = e_byNumber;
    EchartType          type = e_linear;
    TmainLine::EyValue  yValue = TmainLine::e_questionTime;
  };

  explicit Tchart(QQuickItem* parent = nullptr);
  ~Tchart();

  virtual void setAnalyse(EanswersOrder order) {}

  QGraphicsScene* scene;

  void paint(QPainter* painter) override;

      /**
       * 
       */
  void setParentForItem(QGraphicsItem* it);

  void setParentHeight(qreal pH);

  TtipInfo* curQ() { return m_curQ; }

signals:
  void hoveredChanged();

protected:
  void setCurQ(TtipInfo* qa);

protected:
  TXaxis              *xAxis;
  TYaxis              *yAxis;
  QGraphicsRectItem   *p_bgRect;

private:
  qreal               m_parentHeight = 0.0;
  TtipInfo           *m_curQ = nullptr;

};

#endif // TCHART_H
