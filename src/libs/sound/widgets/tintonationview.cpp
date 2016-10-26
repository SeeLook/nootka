/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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
#include "tleavemenu.h"
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <math.h>


/*static*/
float TintonationView::getThreshold(TintonationView::Eaccuracy acc) {
		switch(acc) {
			case e_paranoid: return 0.05;
			case e_perfect: return 0.1;
			case e_normal: return 0.2;
			case e_sufficient: return 0.3;
			case e_dogHowl: return 0.4;
			default: return 0.5;
		}
}


float TintonationView::getThreshold(int accInteger) {
		return getThreshold((Eaccuracy)accInteger);
}
/*end of static*/


TintonationView::TintonationView(int accuracy, QWidget* parent) :
  TabstractSoundView(parent),
  m_pitchDiff(0.0f),
  m_timer(0),
  m_entered(false),
  m_enableAccurChange(true)
{
  setAccuracy(accuracy);
	setMouseTracking(true);
  setMinimumSize(200, 17);
  resizeEvent(0);
}

TintonationView::~TintonationView()
{

}

void TintonationView::setAccuracy(int accuracy) {
  m_accuracy = (Eaccuracy)qBound(0, accuracy, 5);
	m_accurValue = getThreshold(m_accuracy);
  m_accurValue *= INT_FACTOR;
  resizeEvent(0);
}


void TintonationView::setAccuracyChangeEnabled(bool accEnabled) {
  setMouseTracking(accEnabled);
  m_enableAccurChange = accEnabled;
}


/** @p m_pitchDiff is multiplied by INT_FACTOR (1,2) to increase its scale in small values range
* and throw biggest values (near to quarter-tone) form view. 
* m_pitchDiff = (pitch - (float)qRound(pitch)) * 1.2; // this was normal calculation */
void TintonationView::pitchSlot(float pitch) {
	if (m_timer && m_timer->isActive())
		return; // ignore outside calls
	float pitchDiff = getPitchDiff(pitch);
  bool doUpdate = false;
  if (m_pitchDiff != 0.0 || m_pitchDiff != pitchDiff)
      doUpdate = true;
  m_pitchDiff = pitchDiff;
  if (doUpdate)
      repaint();
}


void TintonationView::outOfTuneAnim(float outTune, int duration) {
  Q_UNUSED(duration)
	if (!m_timer) {
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(animationSlot()));
	}
	m_animStep = 0.0; // blinking first
	m_outOfTune = outTune;
	pitchSlot(outTune);
	m_timer->start(150);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TintonationView::enterEvent(QEvent*) {
	if (isEnabled() && m_enableAccurChange) {
		m_entered = true;
		update();
	}
}


void TintonationView::leaveEvent(QEvent*) {
	if (isEnabled() && m_enableAccurChange) {
		m_entered = false;
		update();
	}
}


void TintonationView::mouseMoveEvent(QMouseEvent* e) {
	if (e->x() > (width() - height() * 2) / 2 && e->x() < width() / 2 + height())
		m_overNote = true;
	else
		m_overNote = false;
	update();
}


void TintonationView::paintEvent(QPaintEvent* ) {
  int lastColorThick = (qAbs(m_pitchDiff) / 0.5) * m_ticksCount;
	qMin(lastColorThick, m_tickColors.size() - 1); // it avoids QList bug when m_pitchDiff is near 0.5 (quarter-tone)
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setPen(Qt::NoPen);
  painter.setFont(nootFont);
	QString nSymbol = (isEnabled() && m_accuracy != e_noCheck) ? "n" : "o";
	if (m_entered) {
		QRect nRect = painter.fontMetrics().boundingRect(nSymbol);
		painter.setBrush(m_overNote ? qApp->palette().highlightedText().color().darker(95) : qApp->palette().highlight().color());
		painter.drawRoundedRect((width() - nRect.width() * 2) / 2, 0, nRect.width() * 2, height(), 50, 50, Qt::RelativeSize);
	}
	if (m_entered)
			painter.setPen(m_overNote ? qApp->palette().highlight().color() : qApp->palette().highlightedText().color());
  else if (m_pitchDiff == 0.0)
			painter.setPen(m_enableAccurChange ? tc : disabledColor);
  else
			painter.setPen(m_tickColors[lastColorThick]);
  painter.drawText(0, 0, width(), height(), Qt::AlignCenter, nSymbol);
  for (int i = 0; i < m_ticksCount - 1; i++) {
			QColor thickColor, leftThickColor, rightThickColor;
			if (m_accuracy == e_noCheck)
				thickColor = disabledColor;
			else if (i < lastColorThick)
				thickColor = m_tickColors[i];
			else
				thickColor = tc;
			if (m_accuracy == e_noCheck) {
					leftThickColor = disabledColor;
					rightThickColor = disabledColor;
			} else if (m_pitchDiff < 0) {
					leftThickColor = thickColor; rightThickColor = tc;
			} else {
					leftThickColor = tc; rightThickColor = thickColor; }
			int xx = m_noteX - ((i + 1) * (tickGap() + tickWidth()));
			float yy = (float)i * m_hiTickStep + 1.0f;
			painter.setPen(QPen(leftThickColor, tickWidth(), Qt::SolidLine, Qt::RoundCap));
			painter.drawLine(QLineF(xx, (height() - yy) / 2, xx, height() - (height() - yy) / 2));
			painter.setPen(QPen(rightThickColor, tickWidth(), Qt::SolidLine, Qt::RoundCap));
			xx = (width() - m_noteX) + ((i + 1) * (tickGap() + tickWidth())) - tickWidth();
			painter.drawLine(QLineF(xx, (height() - yy) / 2, xx, height() - (height() - yy) / 2));
  }
}


void TintonationView::resizeEvent(QResizeEvent* ) {
  resizeIt(height());
  m_noteX = (width() - noteBound.width() * 2) / 2;
  m_ticksCount = (m_noteX / (tickWidth() + tickGap()));
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


void TintonationView::mousePressEvent(QMouseEvent* e) {
	if (e->button() == Qt::LeftButton && m_overNote) {
		TleaveMenu menu;
		for (int i = 0; i < 6; ++i) {
			QAction *a = menu.addAction(TintonationCombo::intonationAccuracyTr(Eaccuracy(i)));
			a->setData(i);
			a->setCheckable(true);
			if (m_accuracy == Eaccuracy(i))
				a->setChecked(true);
		}
    QAction *ia = menu.exec(QPoint(QCursor::pos()));
		if (ia){
			setAccuracy(ia->data().toInt());
			m_pitchDiff = 0;
			update();
			emit accuracyChanged();
		}
	}
}


void TintonationView::animationSlot() {
	if (m_animStep < 8.0) { // blinking
		if (m_animStep == 0.0 || m_animStep == 2.0 || m_animStep == 4.0 || m_animStep == 6.0) { // 1 and 3 clips
			m_pitchDiff = 0.01; // green
			repaint(); // pitchSlot is ignored when m_timer is active
		} else {// 2 and 4 clips
				m_pitchDiff = getPitchDiff(m_outOfTune);
				repaint();
		}
		m_animStep += 1.0;
	} else {
			m_timer->stop();
			emit animationFinished();
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
	for (int i = 0; i < 6; i++) {
		QString range = "";
		if (i > 0)
			range = QString::fromUtf8(" (± %1 %2)").arg((int)(TintonationView::getThreshold(i) * 100)).arg(centsText());
		accuracyCombo->addItem(intonationAccuracyTr((TintonationView::Eaccuracy)i) + range);
	}
	setStatusTip(tr(""));
	
	QHBoxLayout *lay = new QHBoxLayout;
    lay->addStretch();
    lay->addWidget(lab);
    lay->addWidget(accuracyCombo);
    lay->addStretch();
	setLayout(lay);
}


QString TintonationCombo::intonationAccuracyTr(TintonationView::Eaccuracy accur) {
	switch (accur) {
			case TintonationView::e_noCheck: return tr("do not check");
			case TintonationView::e_dogHowl: return tr("gums pain", "We used to say that gums or teeth are paining when music is out of tune but it isn't official term.");
			case TintonationView::e_sufficient: return tr("violinist beginner");
			case TintonationView::e_normal: return tr("old strings");
			case TintonationView::e_perfect: return tr("well tuned instrument");
			case TintonationView::e_paranoid: return tr("perfect");		
			default: return "";
	}
}






