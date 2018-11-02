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

#include "tgraphicsline.h"
#include "tgroupedqaunit.h"
#include <exam/texam.h>
#include <exam/textrans.h>

#include <QtWidgets/qapplication.h>


TgraphicsLine::TgraphicsLine(TgroupedQAunit* qaGroup, const QString& text) :
  QGraphicsItem(),
  m_qaGroup(qaGroup)
{
  init(text);
}

TgraphicsLine::TgraphicsLine(const QString& text) :
  QGraphicsItem()
{
  init(text);
}


TgraphicsLine::~TgraphicsLine()
{
  delete m_line;
}

QRectF TgraphicsLine::boundingRect() const {
  return m_line->boundingRect().adjusted(0.0, -2.5, 0.0, 5.0);
}


void TgraphicsLine::setText(const QString& ofSomething) {
  if (m_qaGroup)
      tipText = ofSomething + QLatin1String("<br>") + TexTrans::averAnsverTimeTxt() + QLatin1String("<br>")
      + QLatin1String("<big><b>") + Texam::formatReactTime(qRound(m_qaGroup->averTime()), true) + QLatin1String("</b></big>");
  else
      tipText = ofSomething;
}


void TgraphicsLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  m_line->paint(painter, option, widget);
}


void TgraphicsLine::setDefaultText() {
  tipText = QLatin1String("<b>") + QApplication::translate("TgraphicsLine", "progress line").toUpper() + QLatin1String("</b><br>")
          + QString("<span style=\"color: #00c0c0; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> \\ </b></span>")
          + QApplication::translate("TgraphicsLine", "descending - you are progressing better") + QLatin1String("<br>")
          + QString("<span style=\"color: #00c0c0; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> / </b></span>")
          + QApplication::translate("TgraphicsLine", "ascending - you are thinking too much");
}


void TgraphicsLine::init(const QString& t) {
  kind = e_line;
  p_qaPtr.color = QColor(0, 192, 192);
  if (t.isEmpty())
    setDefaultText();
  else
    tipText = t;
  m_line = new QGraphicsLineItem();
}

