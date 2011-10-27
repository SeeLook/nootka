/***************************************************************************
                          timeaxis.cpp  -  description
                             -------------------
    begin                : Mon Dec 13 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "timeaxis.h"
//#include "mycolor.h"
#include "useful.h"
#include "gdata.h"

#include <math.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

TimeAxis::TimeAxis(QWidget *parent, bool numbersOnTop_)
  : DrawWidget(parent)
{
  _leftTime = 0.0;
  _rightTime = 0.0;
  _numbersOnTop = numbersOnTop_;
  
  init();
}

TimeAxis::TimeAxis(QWidget *parent, double leftTime_, double rightTime_, bool numbersOnTop_)
  : DrawWidget(parent)
{
  _leftTime = leftTime_;
  _rightTime = rightTime_;
  _numbersOnTop = numbersOnTop_;
  init();
}

void TimeAxis::init()
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));

#ifdef MACX
  setFontSize(14);
#else
  setFontSize(12);
#endif
}

TimeAxis::~TimeAxis()
{
}

void TimeAxis::setFontSize(int fontSize)
{
  _fontSize = fontSize;
  _font = QFont("AnyStyle", _fontSize);
}

void TimeAxis::paintEvent(QPaintEvent *)
{
  int frameWidth = 2;
  const int	h = height(), w = width() - 2*frameWidth;
  int fontSpace = _fontSize+2;
  
  beginDrawing(false);
  fillBackground(colorGroup().background());

  double timeStep = timeWidth() / double(w) * 150.0; //time per 150 pixels
  double timeScaleBase = pow10(floor(log10(timeStep))); //round down to the nearest power of 10

  //choose a timeScaleStep which is a multiple of 1, 2 or 5 of timeScaleBase
  int largeFreq;
  if(timeScaleBase * 5.0 < timeStep) { largeFreq = 5; }
  else if (timeScaleBase * 2.0 < timeStep) { largeFreq = 2; }
  else { largeFreq = 2; timeScaleBase /= 2; }
    
  // Draw Ruler Numbers
  p.setBrush(Qt::black);
  //p.setFont(QFont("AnyStyle", h / 2 - 7));
  p.setFont(_font);
  double timePos = floor(leftTime() / (timeScaleBase*largeFreq)) * (timeScaleBase*largeFreq); //calc the first one just off the left of the screen
  int x, largeCounter=-1;
  
  //precalculate line sizes (for efficiency)
  int smallLineTop = 0;
  int smallLineBottom = 0;
  if(_numbersOnTop) {
    smallLineTop = h - 1 - (h - 1 - fontSpace)/2;
    smallLineBottom = h - 1;
  } else {
    smallLineTop = 0;
    smallLineBottom = (h - 1 - fontSpace) / 2;
  }
  int bigLineTop = 0;
  int bigLineBottom = 0;
  if(_numbersOnTop) {
    bigLineTop = fontSpace;
    bigLineBottom = h - 1;
  } else {
    bigLineTop = 0;
    bigLineBottom = h - 1 - fontSpace;
  }
  int textBottom = 0;
  if(_numbersOnTop) textBottom = _fontSize;
  else textBottom = h - 1;
    
  for(; timePos <= rightTime(); timePos += timeScaleBase) {
    if(++largeCounter == largeFreq) {
      largeCounter = 0;
      //draw the bigger lines and the numbers
      //QString numString = QString::number(timePos);

      double newTime = myround(timePos / timeScaleBase) * timeScaleBase;
      QString mins;
      double secs = fmod(newTime, 60.0);

      if (timePos < 0) {
        mins = "-" + QString::number(int(ceil(newTime / 60)));
        secs *= -1;
      } else {
        mins = QString::number(int(floor(newTime / 60)));
      }

      QString seconds = QString::number(secs);
      if (secs < 10 && secs > -10) {
        seconds = "0" + seconds;
      }
      
      QString numString = mins + ":" + seconds;
      x = frameWidth + toInt((timePos-leftTime()) / (timeWidth() / double(w)));
      p.drawText(x - (p.fontMetrics().width(numString) / 2), textBottom, numString);
      p.drawLine(x, bigLineTop, x, bigLineBottom);
    } else {
      //draw the smaller lines
      x = frameWidth + toInt((timePos-leftTime()) / (timeWidth() / double(w)));
      p.drawLine(x, smallLineTop, x, smallLineBottom);
    }
  }
  //draw the horizontal line
  if(_numbersOnTop) {
    p.drawLine(0, h-1, width(), h-1);
  } else {
    p.drawLine(0, 0, width(), 0);
  }
  endDrawing();
}
