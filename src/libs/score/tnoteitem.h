/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TNOTEITEM_H
#define TNOTEITEM_H


#include <nootkacoreglobal.h>
#include "tscoreitem.h"


class Trhythm;


/**
 * Paints note head on the @class TscoreNote
 * and rhythm when it is enabled.
 * It paints steam and flag of whole and quarter notes
 * but if rhythm has any beam,
 * notes above quarter have hidden flags
 *
 * It can be just a cursor without stem and flag when @p setItAsCursor() is called
 */
class NOOTKACORE_EXPORT TnoteItem : public TscoreItem
{

public:
  TnoteItem(TscoreScene* scene, const Trhythm& r);
  virtual ~TnoteItem();

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  virtual QRectF boundingRect() const;

  void setColor(const QColor& c);
  QColor color() { return m_color; }

      /** In cursor mode there are no steam and flag - only note head or a rest */
  void setItAsCursor();


  void setRhythm(const Trhythm& r);
  Trhythm* rhythm() const { return m_rhythm; } /**< Actual rhythm of a note */

  QGraphicsLineItem* stem() { return m_stem; }

      /** Sets stem length - adds @p len value to y1 of stem line */
  void setStemLength(qreal len);

      /** End point (p2) of stem line in parent note coordinates */
  QPointF stemEndPoint() { return pos() + m_stem->line().p2(); }

  void setY(qreal yPos);


private:
  QColor                       m_color;
  Trhythm                     *m_rhythm; /**< This is pointer of @p Trhythm - Note head and flags are determined by it */
  QString                      m_noteLetter; /**< single letter representing a note symbol in Nootka font */
  QGraphicsLineItem           *m_stem;
  QGraphicsSimpleTextItem     *m_flag;
};

#endif // TNOTEITEM_H
