/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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

#include "tscorelines.h"
#include "tscorenote.h"
#include "tscorestaff.h"
#include <QPen>

TscoreLines::TscoreLines(TscoreNote* note) :
	m_parentNote(note)
{
	createLines();
}


void TscoreLines::checkLines(int curPos) {
	for (int i = 0; i < m_upper.size(); i++) {
		if (curPos < m_upper[i]->line().y1())
			m_upper[i]->show();
		else 
			m_upper[i]->hide();
	}
	if (m_parentNote && m_parentNote->staff()->isPianoStaff()) {
		if (curPos == m_middle[0]->line().y1() - 1)
			m_middle[0]->show();
		else
			m_middle[0]->hide();
		if (curPos == m_middle[1]->line().y1() - 1)
			m_middle[1]->show();
		else
			m_middle[1]->hide();
	}
	for (int i = 0; i < m_lower.size(); i++) {
		if (curPos > m_lower[i]->line().y1() - 2) 
			m_lower[i]->show();
		else 
			m_lower[i]->hide();
	}
}


void TscoreLines::hideAllLines() {
	hideLines(m_upper);
	hideLines(m_lower);
	if (m_parentNote && m_parentNote->staff()->isPianoStaff())
		hideLines(m_middle);
}


void TscoreLines::setParent(TscoreNote* sn) {
  m_parentNote = sn;
	for (int i = 0; i < m_upper.size(); i++)
		m_upper[i]->setParentItem(sn);
	for (int i = 0; i < m_middle.size(); i++)
		m_middle[i]->setParentItem(sn);
	for (int i = 0; i < m_lower.size(); i++)
		m_lower[i]->setParentItem(sn);
}



void TscoreLines::setColor(const QColor& lineColor) {
	for (int i = 0; i < m_upper.size(); i++)
        m_upper[i]->setPen(QPen(lineColor, 0.2));
	for (int i = 0; i < m_lower.size(); i++)
      m_lower[i]->setPen(QPen(lineColor, 0.2));
	for (int i = 0; i < m_middle.size(); i++)
      m_middle[i]->setPen(QPen(lineColor, 0.2));
}

//##########################################################################################
//#######################         PRIVATE        ###########################################
//##########################################################################################

QGraphicsLineItem* TscoreLines::createNoteLine(int yPos) {
	QGraphicsLineItem *line = new QGraphicsLineItem();
	line->hide();
	line->setParentItem(m_parentNote);
	line->setZValue(7);
	line->setLine(2.5, yPos, 7.0, yPos);
	return line;
}


void TscoreLines::createLines() {
	deleteLines(m_upper);
	deleteLines(m_middle);
	deleteLines(m_lower);
  int i = m_parentNote->staff()->upperLinePos() - 2;
  while (i > 0) { // upper lines
		m_upper << createNoteLine(i);
    i -= 2;
  }
  i = m_parentNote->staff()->upperLinePos() + 10.0; // distance between upper and lower (or lower grand staff) staff line
  if (m_parentNote->staff()->isPianoStaff()) {
		i = m_parentNote->staff()->lowerLinePos() + 10.0;
		m_middle << createNoteLine(m_parentNote->staff()->upperLinePos() + 10);
		m_middle << createNoteLine(m_parentNote->staff()->lowerLinePos() - 2);
	}
  while (i < m_parentNote->boundingRect().height()) {
		m_lower << createNoteLine(i);
    i += 2;
  }
}


void TscoreLines::deleteLines(TaddLines& linesList) {
	for (int i = 0; i < linesList.size(); i++)
		delete linesList[i];
	linesList.clear();
}


void TscoreLines::hideLines(TaddLines& linesList) {
	for (int i=0; i < linesList.size(); i++)
		linesList[i]->hide();
}





