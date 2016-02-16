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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tgraphicsline.h"
#include "tstatisticstip.h"
#include "tgroupedqaunit.h"
#include <QGraphicsSceneHoverEvent>

TgraphicsLine::TgraphicsLine(TgroupedQAunit* qaGroup, QString text) :
  TtipHandler(),
  m_text(text),
  m_qaGroup(qaGroup)
{
    m_line = new QGraphicsLineItem();
}

TgraphicsLine::TgraphicsLine(QString text) :
  TtipHandler(),
  m_text(text),
  m_qaGroup(0)
{
    m_line = new QGraphicsLineItem();
}


TgraphicsLine::~TgraphicsLine()
{
    delete m_line;
}

QRectF TgraphicsLine::boundingRect() const {
    return m_line->boundingRect();
}

void TgraphicsLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    m_line->paint(painter, option, widget);
}



void TgraphicsLine::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  if (tip) {
    if (tip == initObject())
        return;
    else
      deleteTip();
  }
  if (m_qaGroup)
    tip = new TstatisticsTip(m_qaGroup, TstatisticsTip::e_simple, m_text);
  else {
    QColor averColor = QColor(0, 192, 192);
    if (m_text == "") {
      tip = new TgraphicsTextTip("<b>" + QApplication::translate("TgraphicsLine", "progress line").toUpper() + "</b><br>" +
          QString("<span style=\"color: %1; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> \\ </b></span>").
          arg(averColor.name()) +
          QApplication::translate("TgraphicsLine", "descending - you are progressing better") + "<br>" + 
          QString("<span style=\"color: %1; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> / </b></span>").
          arg(averColor.name()) +
          QApplication::translate("TgraphicsLine", "ascending - you are thinking too much"), averColor);
    } else
      tip = new TgraphicsTextTip(m_text, averColor);
  }
  handleTip(event->scenePos());
}




