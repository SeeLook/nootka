/***************************************************************************
 *   Copyright (C) 2012-2020 by Tomasz Bojczuk                             *
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

#include "txaxis.h"
#include "tgroupedqaunit.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <graphics/tgraphicstexttip.h>
#include <music/tnamestylefilter.h>
#include <tnoofont.h>
#include <QPainter>
#include <QGraphicsScene>
#include <QWidget>
#include <QDebug>



TXaxis::TXaxis(QList<TQAunit*>* answers, Tlevel* level) :
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


void TXaxis::setAnswersList(QList<TQAunit*> *answers, Tlevel* level) {
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

void TXaxis::setTicText(QGraphicsTextItem *tic, TQAunit* unit, int questNr) {
  QString txt;
  if (questNr)
      txt = QString("%1.<br>").arg(questNr);
  QString altStyleText = "";
  if (m_level->requireStyle || (m_level->questionAs.isName() && m_level->answersAs[TQAtype::e_asName].isName())) {
    /** Displays alternate to user pref names but only for levels where different styles can occur */
    Tnote::EnameStyle altStyle;
    if (Tnote::defaultStyle == Tnote::e_italiano_Si || Tnote::defaultStyle == Tnote::e_russian_Ci)
      altStyle = TnameStyleFilter::get(Tnote::e_english_Bb);
    else
      altStyle = TnameStyleFilter::get(Tnote::e_italiano_Si);
    altStyleText = QString(" <small><i>(%1)</small></i>").arg(unit->qa.note.toRichText(altStyle, false));
  }
  if (unit->melody())
// 			txt += "<small>" + QApplication::translate("TXaxis", "%n attempt(s)", "", unit->attemptsCount()) + "</small>";
    txt.replace("<br>", "");
  else {
    txt += QString("<b>%1</b>").arg(unit->qa.note.toRichText()) + altStyleText;
    if (m_level->instrument != e_noInstrument
       && (unit->questionAs == TQAtype::e_asFretPos || unit->answerAs == TQAtype::e_asFretPos || unit->answerAs == TQAtype::e_asSound))
        txt += "<br>" + TnooFont::span(QString::number((int)unit->qa.pos.str()), 15) + 
              QString("<span style=\"font-size: 15px;\">%1</span>").arg(TfingerPos::romanFret(unit->qa.pos.fret()));
  }
  if (m_level->useKeySign &&
    (unit->questionAs == TQAtype::e_asNote || unit->answerAs == TQAtype::e_asNote)) {
      txt += "<br><i>" + unit->key.getName() + "</i>";
  }
  tic->setHtml(txt);
  TgraphicsTextTip::alignCenter(tic);
  if ((tic->boundingRect().width() * scale()) > m_qWidth)
    tic->setScale(((qreal)m_qWidth * scale()) / tic->boundingRect().width());
}


void TXaxis::setAnswersLists(QList<TgroupedQAunit>& listOfLists, Tlevel* level) {
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
      setTicText(ticTip, listOfLists[i].operator[](j).qaPtr);
      scene()->addItem(ticTip);
      ticTip->setPos(pos().x() + mapValue(cnt) - ticTip->boundingRect().width() / 2 , pos().y() + 15);
      m_ticTips << ticTip;
    }
  }      
}


void TXaxis::setAnswersForBarChart(QList<TgroupedQAunit>& listOfLists) {
  setLength(m_qWidth * (listOfLists.size() + 1));
  update(boundingRect());
  m_ticTips.clear();
  for (int i = 0; i < listOfLists.size(); i++) {
    QGraphicsTextItem *ticTip = new QGraphicsTextItem();
    ticTip->setHtml(listOfLists[i].description());
    TgraphicsTextTip::alignCenter(ticTip);
    if ((ticTip->boundingRect().width() * scale()) > m_qWidth)
      ticTip->setScale(((qreal)m_qWidth * scale()) / ticTip->boundingRect().width());
    scene()->addItem(ticTip);
		ticTip->setPos(pos().x() + mapValue(i + 1) - (ticTip->boundingRect().width() * ticTip->scale()) / 2 , pos().y() + 15);
    m_ticTips << ticTip;
  }
}


void TXaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {  
  Q_UNUSED(option)
      
  qreal half = axisWidth /  2.0;
  painter->setPen(QPen(widget->palette().text().color(), 2));
  painter->drawLine(0, half, length(), half);
  drawArrow(painter, QPointF(length(), half), true);
  
  int b = length() / m_qWidth -1;
  for (int i = 1; i <= b; i++) {
    painter->drawLine(i*m_qWidth, half, i*m_qWidth, half + tickSize);
  }
  
}

QRectF TXaxis::boundingRect()
{
  QRectF rect(0, 0, length(), axisWidth);
  return rect;
}

