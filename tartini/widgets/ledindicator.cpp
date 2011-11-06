/***************************************************************************
                          ledindicator.cpp  -  description
                             -------------------
    begin                : Tue Jan 11 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "ledindicator.h"

#include <qpixmap.h>
//Added by qt3to4:
#include <QPaintEvent>

LEDIndicator::LEDIndicator(QPixmap *buffer, QWidget *parent, const char *name, const QColor &on, const QColor &off)
: QWidget(parent, name)
{
  setMinimumSize(sizeHint());
  this->on = on;
  this->off = off;

  active = false;

  // Stop QT from erasing the background all the time
  setBackgroundMode(Qt::NoBackground);
  this->buffer = buffer;
}

LEDIndicator::~LEDIndicator()
{
}

void LEDIndicator::setOn(bool on) {
	if (active != on) {
		active = on;
		update();
	}
}

void LEDIndicator::toggle() {
  active = !active;
}

bool LEDIndicator::lit() {
  return active;
}

void LEDIndicator::paintEvent(QPaintEvent *)
{
  // Double buffering
  if (buffer->size() != size()) {
      buffer->resize(size());
  }

  buffer->fill(colorGroup().background());
  
  QPainter p;
  p.begin(buffer, this);

  p.fillRect(0, 0, QWidget::width(), QWidget::height(), (active) ? on : off);

  p.setPen(colorGroup().brightText());

  QFontMetrics fm = p.fontMetrics();
  int fontHeight = fm.height() / 4;
  int fontWidth = fm.width(name()) / 2;
  
  p.drawText(QWidget::width()/2 - fontWidth, QWidget::height()/2 + fontHeight, name());
  p.end();

  // Swap buffers
 	bitBlt(this, 0, 0, buffer);
}
