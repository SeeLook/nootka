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

#include "txaxis.h"
#include "tqaunit.h"
#include "texam.h"
#include "tnotename.h"
#include "texamlevel.h"
#include "tgraphicstexttip.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>



TXaxis::TXaxis(QList< TQAunit >* answers, TexamLevel* level) :
  m_qWidth(70)
{
  if (answers && level)
      setAnswersList(answers, level);
  else {
      setLength(100);
      m_answers = 0;
      m_level = 0;
  }
  axisScale = m_qWidth;
}

TXaxis::~TXaxis()
{}


void TXaxis::setAnswersList(QList< TQAunit >* answers, TexamLevel* level) {
  m_answers = answers;
  m_level = level;
  setLength(m_qWidth * (m_answers->size() + 1));
  update(boundingRect());
  m_ticTips.clear();
  for (int i = 0; i < m_answers->size(); i++) {
      QGraphicsTextItem *ticTip = new QGraphicsTextItem();
      setTicText(ticTip, m_answers->operator[](i), i + 1);
      scene()->addItem(ticTip);
      ticTip->setPos(pos().x() + mapValue(i+1) - ticTip->boundingRect().width() / 2 , pos().y() + 15);
      m_ticTips << ticTip;
  }
}

void TXaxis::setTicText(QGraphicsTextItem *tic, TQAunit &unit, int questNr) {
    QString txt;
    if (questNr)
        txt = QString("%1.<br>").arg(questNr);
    txt += QString("<b>%2</b>").arg(TnoteName::noteToRichText(unit.qa.note));
    if (unit.questionAs == TQAtype::e_asFretPos || unit.answerAs == TQAtype::e_asFretPos || unit.answerAs == TQAtype::e_asSound)
        txt += "<br>" + QString("<span style=\"font-size: 15px; font-family: nootka\">%1</span><span style=\"font-size: 15px;\">%2</span>").
                arg((int)unit.qa.pos.str()).arg(TfingerPos::romanFret(unit.qa.pos.fret()));
    if (m_level->useKeySign &&
      (unit.questionAs == TQAtype::e_asNote || unit.answerAs == TQAtype::e_asNote)) {
        txt += "<br><i>" + unit.key.getName() + "</i>";
    }
    tic->setHtml(txt);
    TgraphicsTextTip::alignCenter(tic);
    if ((tic->boundingRect().width() * scale()) > m_qWidth)
      tic->setScale(((qreal)m_qWidth * scale()) / tic->boundingRect().width());
      
}

void TXaxis::setAnswersLists(QList< QList< TQAunit* > >& listOfLists, TexamLevel* level) {
  int ln = 0, cnt = 0;
  m_level = level;
  for (int i = 0; i < listOfLists.size(); i++) {
    ln += listOfLists[i].size();
  }
  setLength(m_qWidth * (ln + 1));
  update(boundingRect());
  m_ticTips.clear();
  for (int i = 0; i < listOfLists.size(); i++) {
    for (int j = 0; j < listOfLists[i].size(); j++) {
      cnt++;
      QGraphicsTextItem *ticTip = new QGraphicsTextItem();
      setTicText(ticTip, *listOfLists[i].operator[](j));
      scene()->addItem(ticTip);
      ticTip->setPos(pos().x() + mapValue(cnt) - ticTip->boundingRect().width() / 2 , pos().y() + 15);
      m_ticTips << ticTip;
    }
  }      
}



void TXaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  
    Q_UNUSED(option)
    Q_UNUSED(widget)
      
  qreal half = axisWidth /  2.0;
  painter->drawLine(0, half, length(), half);
  drawArrow(painter, QPointF(length(), half), true);
  
  int b = length() / m_qWidth -1;
  for (int i=1; i <= b; i++) {
    painter->drawLine(i*m_qWidth, half, i*m_qWidth, half + tickSize);
  }
  
}

QRectF TXaxis::boundingRect()
{
  QRectF rect(0 ,0, length(), axisWidth);
//  rect.translate(1, axisWidth / 2.0);
  return rect;
}

