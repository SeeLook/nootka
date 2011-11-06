/***************************************************************************
                          paino.cpp  -  description
                             -------------------
    begin                : 17 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "pianowidget.h"
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include "drawwidget.h"
#include "gdata.h"
#include "myqt.h"

static int isBlackKeyArray[12] = { 1, -1, 2, -2, 3, 4, -3, 5, -4, 6, -5, 7 };

PianoWidget::PianoWidget(QWidget *parent) : DrawWidget(parent)
{
  setNoNote();
}

PianoWidget::~PianoWidget()
{
}

void PianoWidget::setCurrentNote(int n, float amountPressed)
{
  if(n >= 0 && n < 12) _currentNote = n;
  else setNoNote();
  _amountPressed = amountPressed;
}

void PianoWidget::paintEvent(QPaintEvent *)
{
  int j;
  beginDrawing(false);
  fillBackground(Qt::white);
  
  double scaleX = double(width()) / 1650.0; //1650mm
  double scaleY = double(height()) / 1480.0; //1480mm
  //p.setWindow(0, 0, 1650, 1480); //measurements in 10th's of a mm from a actual piano
  
  if(isNote()) {
    int notepart = isBlackKeyArray[currentNote()];
    if(notepart >= 0) { //it's a white key
      p.fillRect(toInt((notepart-1)*235.0*scaleX), 0, toInt(234.0*scaleX)+1, height(), colorBetween(Qt::white, Qt::red, _amountPressed));
    }
  }
  
  p.setPen(Qt::black);
  //draw the lines between the white keys
  for(j = 1; j<7; j++) {
    //p.moveTo(toInt(double(j)*235.0*scaleX), 0);
    //p.lineTo(toInt(double(j)*235.0*scaleX), height());
    p.drawLine(toInt(double(j)*235.0*scaleX), 0, toInt(double(j)*235.0*scaleX), height());
  }

  //draw the black keys
  double blackKeyOffset[5] = { 150.0, 430.0, 855.0, 1125.0, 1390.0 };
  for(j=0; j<5; j++) {
    p.fillRect(toInt(blackKeyOffset[j]*scaleX), 0, toInt(110.0*scaleX), toInt(1000.0*scaleY), Qt::black);
  }
  
  if(isNote()) {
    int notepart = isBlackKeyArray[currentNote()];
    if(notepart < 0) { //it's a black key
      p.fillRect(toInt(blackKeyOffset[-notepart-1]*scaleX), 0, toInt(110.0*scaleX), toInt(1000.0*scaleY), colorBetween(Qt::black, Qt::red, _amountPressed));
    }
  }
  endDrawing();
}
