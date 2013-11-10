/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tcoloredanim.h"
#include <qt4/QtGui/QPen>

TcoloredAnim::TcoloredAnim(QGraphicsItem* item, QObject* parent) :
	TabstractAnim(item, parent),
	m_line(0), m_richText(0), m_ellipse(0), m_text(0)
{
	QGraphicsLineItem *tmpline = new QGraphicsLineItem;
	QGraphicsTextItem *tmpRichText = new QGraphicsTextItem;
	QGraphicsEllipseItem *tmpEll = new QGraphicsEllipseItem;
	QGraphicsSimpleTextItem *tmpText = new QGraphicsSimpleTextItem;
	if (item->type() == tmpline->type()) {
			m_line = qgraphicsitem_cast<QGraphicsLineItem*>(item);
			m_startColor = m_line->pen().color();
	} else if (item->type() == tmpRichText->type()) {
			m_richText = qgraphicsitem_cast<QGraphicsTextItem*>(item);
			m_startColor = m_richText->defaultTextColor();
	} else if (item->type() == tmpEll->type()) {
			m_ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
			m_startColor = m_ellipse->brush().color();
	} else if (item->type() == tmpText->type()) {
			m_text = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
			m_startColor = m_text->brush().color();
	}
	delete tmpline;
	delete tmpRichText;
	delete tmpEll;
	delete tmpText;
}


void TcoloredAnim::startColoring(const QColor& endColor) {
	m_endColor = endColor;
	initAnim();
}



void TcoloredAnim::animationRoutine() {
		nextStep();
		if (currentStep() <= stepsNumber()) {
			QColor newC;
			qreal easy = easyValue((qreal)currentStep() / (qreal)stepsNumber());
			newC.setRed(qRound(m_startColor.red() + easy * (m_endColor.red() - m_startColor.red())));
			newC.setGreen(qRound(m_startColor.green() + easy * (m_endColor.green() - m_startColor.green())));
			newC.setBlue(qRound(m_startColor.blue() + easy * (m_endColor.blue() - m_startColor.blue())));
			newC.setAlpha(qRound(m_startColor.alpha() + easy * (m_endColor.alpha() - m_startColor.alpha())));
			if (m_line)
				m_line->setPen(QPen(newC, m_line->pen().widthF()));
			else if (m_richText)
				m_richText->setDefaultTextColor(newC);
			else if (m_ellipse)
				m_ellipse->setBrush(QBrush(newC));
			else if (m_text)
				m_text->setBrush(QBrush(newC));
		} else
				stopAnim();
}




