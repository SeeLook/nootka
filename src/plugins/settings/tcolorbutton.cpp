/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   		      				                                 *
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


#include "tcolorbutton.h"
#include <QtWidgets/QtWidgets>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
  #include "tcolordialog.h"
#endif


TcolorButton::TcolorButton(QColor col, QWidget* parent): 
	QPushButton(parent)
{
  m_color = col;
  m_color.setAlpha(255);
#if defined (Q_OS_ANDROID)
  setFixedSize(Tmtr::fingerPixels(), Tmtr::fingerPixels() * 0.7);
#else
  setFixedSize(40, 30);
#endif
  connect(this, SIGNAL(clicked()), this, SLOT(whenClicked()));
}


void TcolorButton::whenClicked() {
#if defined (Q_OS_ANDROID)
  TcolorDialog cd(m_color, this);
  if (cd.exec() == TcolorDialog::Accepted)
    setColor(cd.selectedColor());
#else
  QColor userColor = QColorDialog::getColor(m_color, this, "");
  if (userColor.isValid())
      setColor(userColor);
#endif
}


void TcolorButton::setColor(QColor col) {
  m_color = col;
  m_color.setAlpha(255);
  repaint();
}


void TcolorButton::paintEvent(QPaintEvent* event) {
  QPushButton::paintEvent(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(palette().text().color()));
  painter.drawRect(5, 5, width() - 8, height() - 10);
  if (isEnabled())
    painter.setBrush(QBrush(m_color));
  else
    painter.setBrush(QBrush(palette().color(QPalette::Disabled, QPalette::Text)));
  painter.drawRect(4, 4, width() - 8, height() - 10);
}




