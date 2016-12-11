/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include "tabstractsoundview.h"
#include <music/ttune.h>
#include <music/tnotestruct.h>
#include <math.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qscreen.h>


QColor TabstractSoundView::startColor = Qt::green;
QColor TabstractSoundView::middleColor = Qt::yellow;
QColor TabstractSoundView::endColor = Qt::red;
QColor TabstractSoundView::totalColor = QColor(117, 21, 86); // brown
QColor TabstractSoundView::disabledColor = Qt::gray;
int    TabstractSoundView::m_tickWidth = 0;
int    TabstractSoundView::m_tickGap = 0;


/**
 * It adds line break tag after every two entries
 * and ALT_BR text after every three.
 * This way it can be easily split into 
 */
QString TabstractSoundView::getStringsFreqText(Ttune* t, float pitch440Offset) {
  QString freqTxt;
  QString br = QStringLiteral("<br>");
  QString altBr = QStringLiteral("ALT_BR");
  if (t->stringNr() > 2) { // guitar
    for (int i = 1; i <= t->stringNr(); i++) {
      float offPitch = TnoteStruct::pitchToFreq(t->str(i).toMidi() + pitch440Offset);
      freqTxt += QString("<span style=\"font-family: nootka;\">%1</span>%2 = %3 Hz, ").arg(i).arg((t->str(i)).toRichText()).arg(offPitch, 0, 'f', 1);
      if (i % 2 == 0 && i < 6)
          freqTxt += br; // two entries per line
      else if (i % 3 == 0 && i < 6)
          freqTxt += altBr; // three entries per line
    }
  } else { // no guitar - C-major scale frequencies
    for (int i = 1; i < 8; i++) {
      float offPitch = TnoteStruct::pitchToFreq(t->str(i).toMidi() + pitch440Offset);
      freqTxt += QString("<b>%1</b> = %2 Hz, ").arg(Tnote(i, 1, 0).toRichText()).arg(offPitch, 0, 'f', 1);
      if (i % 2 == 0 && i < 7)
          freqTxt += br; // three entries per line
      else if (i % 3 == 0 && i < 7)
          freqTxt += altBr; // four entries per line
    }
  }
  return freqTxt;
}



TabstractSoundView::TabstractSoundView(QWidget* parent) :
  QWidget(parent)
{
  nootFont = QFont(QStringLiteral("nootka"));
  nootFont.setBold(false);
  tc = palette().color(palette().currentColorGroup(), QPalette::Text);
  disabledColor = palette().color(QPalette::Disabled, QPalette::Text);
  disabledColor.setAlpha(150);
#if defined (Q_OS_MAC)
    m_tickWidth = 2;
    m_tickGap = 2;
#else
  if (m_tickWidth == 0) {
    m_tickWidth = qRound((qApp->screens().first()->physicalDotsPerInchX() / 22.0) * 0.4);
    m_tickGap = qRound(m_tickWidth * 1.25);
#endif
  }

}


void TabstractSoundView::setDisabled(bool isDisabled){
  QWidget::setDisabled(isDisabled);
  if (isDisabled)
    tc = disabledColor;
	else
    tc = palette().color(QPalette::Active, QPalette::Text);
}


    /** Implementation of linear gradient color at given point taken from:
      http://www.qtcentre.org/threads/14307-How-to-get-the-specified-position-s-QColor-in-QLinearGradient */
QColor TabstractSoundView::gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC) {
  float segmentLength = sqrt((lineX2 - lineX1) * (lineX2 - lineX1));
  double pdist = sqrt((posC - lineX1) * (posC - lineX1));
  double ratio = pdist / segmentLength;
  int red = qBound(0, (int)(ratio * endC.red() + ( 1 - ratio) * startC.red()), 255);
  int green = qBound(0, (int)(ratio * endC.green() + (1 - ratio) * startC.green()), 255);
  int blue = qBound(0, (int)(ratio * endC.blue() + (1 - ratio) * startC.blue()), 255);
  return QColor(red, green, blue);
}


void TabstractSoundView::resizeIt(int myHeight) {
  nootFont.setPointSizeF(20.0);
  QFontMetrics fm(nootFont);
  noteBound = fm.boundingRect("o");
  float factor = (float)myHeight / (float)noteBound.height();
  nootFont.setPointSizeF(20.0 * (((float)myHeight * 0.95) / (float)noteBound.height()));
  noteBound = fm.boundingRect("o");
}


