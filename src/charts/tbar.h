/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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

#ifndef TBAR_H
#define TBAR_H


#include "tgroupedqaunit.h"
#include <QtWidgets/qgraphicsitem.h>


#define BAR_TYPE (65615)


class TgroupedQAunit;
class QStyleOptionGraphicsItem;
class QWidget;


class Tbar : public TtipInfo, public QGraphicsObject
{

public:

        /** Determines how many data are shown in tip
         * @p  e_full - (default) average time, questions number and all kinds of mistakes.
         * @p e_simple - average time only (It is necessary to give description in constructor). 
         * @p e_mistakes - sort by mistakes, skips effectiveness and questions number
         */
    enum Ekind { e_full, e_simple, e_mistakes };

  Tbar(qreal height, TgroupedQAunit* qaGroup, Tbar::Ekind tipType = Tbar::e_full);
  virtual ~Tbar();

  enum { Type = UserType + 79 }; /**< 65615 */
  int type() const override { return Type; }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
  QRectF boundingRect() const override;

private:
  qreal               m_height;
  TgroupedQAunit     *m_qaGroup;
  qreal               m_wrongAt, m_notBadAt; /**< Keeps position of color gradient for mistakes */
};

#endif // TBAR_H
