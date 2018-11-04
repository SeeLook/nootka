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

#ifndef TGRAPHICSLINE_H
#define TGRAPHICSLINE_H


#include "tgroupedqaunit.h"
#include <QtGui/qpen.h>
#include <QtWidgets/qgraphicsitem.h>


#define CHART_LINE_TYPE (65614)

/**
 * This class represents a line on QGraphicsScene
 * but it has other type to be handled by chart hover events
 */
class TgraphicsLine : public TtipInfo, public QGraphicsItem
{

public:
  TgraphicsLine(TgroupedQAunit* qaGroup, const QString& text = QString());
  TgraphicsLine(const QString& text = QString());
  virtual ~TgraphicsLine();

  QString text() const { return tipText; }

      /**
       * Sets line tip text to @P ofSomething
       * When @p TgraphicsLine has @p TgroupedQAunit appropriate sentence with time of the unit is added
       */
  void setText(const QString& ofSomething);
  void setPen(const QPen& pen) { m_line->setPen(pen); }
  void setLine(qreal x1, qreal y1, qreal x2, qreal y2) { m_line->setLine(x1, y1, x2, y2); }

  enum { Type = UserType + 78 }; /**< 65614 */
  int type() const { return Type; }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);
  virtual QRectF boundingRect() const;

protected:
  void setDefaultText();
  void init(const QString& t);

private:
  TgroupedQAunit     *m_qaGroup = nullptr;
  QGraphicsLineItem  *m_line;

};

#endif // TGRAPHICSLINE_H
