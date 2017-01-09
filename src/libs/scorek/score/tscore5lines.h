/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TSCORE5LINES_H
#define TSCORE5LINES_H

#include "nootkacoreglobal.h"
#include "tscoreitem.h"

/**
 * This class represents five staff lines
 */
class NOOTKACORE_EXPORT Tscore5lines : public TscoreItem
{

public:
  Tscore5lines(TscoreScene* scene);

  void setWidth(qreal w); /** Staff lines width in scene coordinates. */
  qreal width() { return m_width; }
  qreal height() { return m_height; }

  void setPianoStaff(bool isPiano); /** Adds grand staff lines or deletes them */
  bool isPianoStaff() { return m_isPianoStaff; }

  void setDisabled(bool dis); /** Disables lines making them translucent and blurred */

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {}
  virtual QRectF boundingRect() const;

private:
  void createLines(QList<QGraphicsLineItem*>& lines);

private:
  QList<QGraphicsLineItem*>          m_lines, m_lowLines; // five staff lines
  qreal                              m_width, m_height;
  bool                               m_isPianoStaff;
};

#endif // TSCORE5LINES_H
