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


#include "tintonationview.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDebug>
#include <math.h>


#define TICK_WIDTH (2)
#define TICK_GAP (3)
#define INT_FACTOR (1.2) 

TintonationView::TintonationView(int accuracy, QWidget* parent) :
  TabstractSoundView(parent),
  m_pitchDiff(0.0f)
{
  setAccuracy(accuracy);
  setMinimumSize(200, 17);
  resizeEvent(0);
}

TintonationView::~TintonationView()
{

}

void TintonationView::setAccuracy(int accuracy) {
  m_accuracy = (Eaccuracy)qBound(0, accuracy, 5);
  switch(m_accuracy) {
		case e_paranoid:
      m_accurValue = 0.05; break;
    case e_perfect:
      m_accurValue = 0.1; break;
    case e_normal:
      m_accurValue = 0.2; break;
    case e_sufficient:
      m_accurValue = 0.3; break;
		case e_dogHowl:
      m_accurValue = 0.4; break;
		default: m_accurValue = 0.5;
  }
  m_accurValue *= INT_FACTOR;
  resizeEvent(0);
}


void TintonationView::pitchSlot(float pitch) {
  bool doUpdate = false;
  if (m_pitchDiff != 0.0)
      doUpdate = true;
			/** @p m_pitchDiff is multiplied by INT_FACTOR (1,2) to increase its scale in small values range
			 * and throw biggest values (near to quarter-tone) form view. */
  m_pitchDiff = qBound(-0.49, (pitch - (float)qRound(pitch)) * INT_FACTOR, 0.49);
// 	m_pitchDiff = (pitch - (float)qRound(pitch)) * 1.2;
  if (doUpdate)
      update();
}


//################################################################################
//############################### protected ######################################
//################################################################################


void TintonationView::paintEvent(QPaintEvent* ) {
  int lastColorThick = (qAbs(m_pitchDiff) / 0.5) * m_ticksCount;
// 	if (lastColorThick > m_tickColors.size() - 1) {
// 		qDebug() << "wrong calc. in intonation";
// 		return;
// 	}
	qMin(lastColorThick, m_tickColors.size() - 1); // it avoids QList bug when m_pitchDiff is near 0.5 (quarter-tone)
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
	QColor bg = palette().window().color();
	bg.setAlpha(200);
	painter.setBrush(bg);
	if (m_pitchDiff == 0.0) {
			painter.setPen(Qt::NoPen);
	} else {
			painter.setPen(QPen(m_tickColors[lastColorThick], 3.0));
	}
  painter.drawRoundedRect(painter.viewport(), 4, 4);
  if (m_pitchDiff == 0.0)
			painter.setPen(QPen(tc));
  else
			painter.setPen(QPen(m_tickColors[lastColorThick]));
  painter.setFont(nootFont);
  painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "n");
  for (int i = 0; i < m_ticksCount - 1; i++) {
			QColor thickColor, leftThickColor, rightThickColor;
			if (i < lastColorThick)
				thickColor = m_tickColors[i];
			else
				thickColor = tc;
			if (m_pitchDiff < 0) {
					leftThickColor = thickColor; rightThickColor = tc;
			} else {
					leftThickColor = tc; rightThickColor = thickColor;
			}
			int xx = m_noteX - ((i + 1) * (TICK_GAP + TICK_WIDTH));
			float yy = (float)i * m_hiTickStep + 1.0f;
	//     int yy = 1;
			painter.setPen(QPen(leftThickColor, TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
			painter.drawLine(QLineF(xx, (height() - yy) / 2, xx, height() - (height() - yy) / 2));
			painter.setPen(QPen(rightThickColor, TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
			xx = (width() - m_noteX) + ((i + 1) * (TICK_GAP + TICK_WIDTH)) - TICK_WIDTH;
			painter.drawLine(QLineF(xx, (height() - yy) / 2, xx, height() - (height() - yy) / 2));
  }
}


void TintonationView::resizeEvent(QResizeEvent* ) {
  resizeIt(height());
  m_noteX = (width() - noteBound.width() * 2) / 2;
  m_ticksCount = (m_noteX / (TICK_WIDTH + TICK_GAP));
  m_hiTickStep = ((float)height() * 0.66) / m_ticksCount;
  m_tickColors.clear();
  for (int i = 0; i < m_ticksCount; i++) {
    if (i <= (m_ticksCount * m_accurValue) ) {
      m_tickColors << gradColorAtPoint(0, m_noteX * m_accurValue * 2, startColor, middleColor, (i + 1) * (m_noteX / m_ticksCount));
    }
    else if (i <= (m_ticksCount * (m_accurValue + 0.3)) ) {
      m_tickColors << gradColorAtPoint(m_noteX * m_accurValue, m_noteX * (m_accurValue + 0.4), 
																			 middleColor, endColor, (i + 1) * (m_noteX / m_ticksCount));
		} else {
			m_tickColors << gradColorAtPoint(m_noteX * (m_accurValue + 0.3), m_noteX, endColor, totalColor, (i + 1) * (m_noteX / m_ticksCount));
		}
  }
}


//===============================================================================================================
//############################### TintonationCombo ##############################################################
//===============================================================================================================

TintonationCombo::TintonationCombo(QWidget* parent) : 
	QWidget(parent)
{
	QLabel *lab = new QLabel(tr("intonation accuracy"), this);
	accuracyCombo = new QComboBox(this);
	int accurArray[6] = {0, 40, 30, 20, 10, 5};
	for (int i = 0; i < 6; i++) {
		QString range = "";
		if (i > 0)
			range = QString::fromUtf8(" (± %1 %2)").arg(accurArray[i]).arg(centsText());
		accuracyCombo->addItem(intonationAccuracyTr((TintonationView::Eaccuracy)i) + range);
	}
	setStatusTip(tr(""));
	
	QHBoxLayout *lay = new QHBoxLayout;
	lay->addWidget(lab);
	lay->addWidget(accuracyCombo);
	setLayout(lay);
}


QString TintonationCombo::intonationAccuracyTr(TintonationView::Eaccuracy accur) {
	switch (accur) {
			case TintonationView::e_noCheck: return tr("do not check");
			case TintonationView::e_dogHowl: return tr("gum pain");
			case TintonationView::e_sufficient: return tr("violinist beginner");
			case TintonationView::e_normal: return tr("old strings");
			case TintonationView::e_perfect: return tr("well tuned guitar");
			case TintonationView::e_paranoid: return tr("perfect");		
			default: return "";
	}
}






