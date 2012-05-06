/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#include "ttipchart.h"
#include "tqaunit.h"
#include "texamview.h"
#include "tquestionaswdg.h"
// #include <QGraphicsScene>
#include <QPainter>
#include <qstyleoption.h>
#include <QApplication>
#include <qgraphicseffect.h>
#include <QDebug>


TtipChart::TtipChart(TQAunit* question) :
  QGraphicsTextItem()
{
  QString txt = "<b>" + TquestionAsWdg::questionTxt() + " ";
  switch (question->questionAs) {
    case TQAtype::e_asNote : txt += TquestionAsWdg::asNoteTxt(); break;
    case TQAtype::e_asName : txt += TquestionAsWdg::asNameTxt(); break;
    case TQAtype::e_asFretPos : txt += TquestionAsWdg::asFretPosTxt(); break;
    case TQAtype::e_asSound : txt += TquestionAsWdg::asSoundTxt(); break;    
  }
  
  txt += "</b><br>";
  txt += TexamView::reactTimeTxt() + "<br>" + 
        QString("<span style=\"font-size: 20px\">%1s</span>").arg((double)question->time / 10.0);
//   m_text = new QGraphicsTextItem();
//   this->scene()->addItem(m_text);
  
  setHtml(txt);
  
  
  setZValue(75);
//   m_text->setZValue(76);
  
  setFlag(QGraphicsItem::ItemIgnoresTransformations);
  
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(5);
  shadow->setOffset(5, 5);
  setGraphicsEffect(shadow);
//   m_text->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

TtipChart::~TtipChart() {
//   scene()->removeItem(m_text);
//   qDebug() << "delete" << m_text;
//   delete m_text;
//   m_text = 0;
}



void TtipChart::setPos(QPointF p) {
  QGraphicsItem::setPos(p);
//   m_text->setPos(p);
}



void TtipChart::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
//   Q_UNUSED(option)
//   Q_UNUSED(widget)


  
	QRectF rect = boundingRect();
//   painter->setBrush(QBrush(Qt::lightGray));
//   QColor shadow = option->palette.dark().color();
//   shadow.setAlpha(170);
	painter->setPen(Qt::NoPen);
//   painter->setBrush(shadow);
//   painter->drawRoundedRect(rect.x() + 7 ,rect.y() + 7, rect.width(), rect.height(), 7, 7);
	painter->setBrush(QBrush(option->palette.light()));
	painter->drawRoundedRect(rect, 5, 5);
  
	QGraphicsTextItem::paint(painter, option, widget);  

}

QRectF TtipChart::boundingRect() const {
//   if (m_text)
    return QGraphicsTextItem::boundingRect();
//   else
//     return QRectF(0, 0, 1, 1);
}



