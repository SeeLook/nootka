/***************************************************************************
                          tunerwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QPaintEvent>

#include "tunerwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

TunerWidget::TunerWidget(QWidget *parent)
  : DrawWidget(parent)
{
  value_ = 0;
}

TunerWidget::~TunerWidget()
{
}

/*
int TunerWidget::heightForWidth( int width ) const
{
  return width;
}
*/
void TunerWidget::paintEvent( QPaintEvent * )
{
  beginDrawing();

  /*
    The tuner is basically just a sector. It has a radius of 1.2 * height() or 
	1.2 * width() when appropriate, and we draw the largest sector we can fit on screen.

    To calculate this sector, we have to work out the starting angle, and work out how
    much we should draw. From basic trig, we can calculate the starting angle:

    <--- w ---->
    ___________
    \    |    /             sin t = Opposite / Hypotenuse
     \   |   /                    = 0.5 width() / radius
      \  |  / r = 1.2h (or 1.2w)  = width() / 2 * radius
       \ |t/
        \|/

    So the starting angle is 90 - t. The finishing angle is just the starting angle + 2t.

    To draw the markers, we can just use the equation of a circle to get x and y points
    along it.

    cos(i) = x / r   ->  x = r * cos(i)
    sin(i) = y / r   ->  y = r * sin(i)

    These points have to be adjusted to fit in the widget, so the x co-ordinates get
    radius added to them, and the y co-ordinates are taken away from radius.

    From there, we just step along the edge, drawing a line at each point we want. The
    lines would eventually meet up with the radius of the circle (width()/2, radius).

  */

  QPen pen(colorGroup().foreground(), 2);
  p.setPen(pen);  // Border

  double halfWidth = double(width()) / 2.0;
  double radius = 1.8 * MAX(height()/2, halfWidth);
  QPoint center(toInt(halfWidth), toInt(radius));
  double theta = asin(double(width()) / (2 * radius));
  double thetaDeg = theta *180.0 / PI;
  double rho = (PI / 2.0) - theta;

  {// Draw the semicircle
    p.setBrush(Qt::white);  // Fill colour
    //p.drawPie(width() / 2 - radius, 0, 2 * radius, 2 * radius, toInt((90 - thetaDeg) * 16), toInt(2*thetaDeg*16));
    p.drawPie(toInt(halfWidth - radius), 0, toInt(2.0 * radius), toInt(2.0 * radius), toInt((90 - thetaDeg) * 16), toInt(2*thetaDeg*16));
    //p.setBrush(colorGroup().background());
    p.drawArc(toInt(halfWidth - (radius/2.0)), toInt(radius/2.0), toInt(radius), toInt(radius), toInt((90 - thetaDeg) * 16), toInt(2*thetaDeg*16));
  }
 
  p.setPen(colorGroup().foreground());
  p.setBrush(colorGroup().foreground());
  
  //p.drawEllipse(width() / 2 - halfKnobWidth, radius- halfKnobWidth, 2*halfKnobWidth, 2*halfKnobWidth);
    
  double step = (2 * theta) / 12.0;
  double stop = rho + (2 * theta) - (step / 2);
  {// Draw line markings
    for (double i = rho + step; i < stop; i += step) {
      int x = toInt(halfWidth + radius * cos(i));
      int y = toInt(radius - radius * sin(i));
      QPoint start(x, y);
      // p = start + center *t
      double t = 0.05; //0.025;
      p.drawLine(start, start + t * (center - start));
    }
  }
  
  {//Draw the text labels
    p.setPen(colorGroup().foreground());
  
    const char *theNames[11] = { "+50", "+40", "+30", "+20", "+10", "0", "-10", "-20", "-30", "-40", "-50" };
    QFontMetrics fm = p.fontMetrics();
    int halfFontHeight = fm.height() / 2;
    int halfFontWidth;
    
    for (int j=0; j<11;) {
      double i = rho + step*(j+1);
      int x = toInt(halfWidth + radius * cos(i));
      int y = toInt(radius - radius * sin(i));
      QPoint start(x, y);
      double t = 0.08; //0.025;
      QPoint pt = start + t * (center - start);
      halfFontWidth = fm.width(theNames[j]) / 2;
      
      p.drawText(pt.x() - halfFontWidth, pt.y() + halfFontHeight, theNames[j]);
      if(radius < 300) j+=2; else j++;
    }
    halfFontWidth = fm.width("Cents") / 2;
    p.drawText(center.x() - halfFontWidth, toInt(center.y() * 0.2) + halfFontHeight, "Cents");
  }
    
  { //draw needle
    double centAngle = (2*theta) / 120;
    double note = rho + (fabs(value_ - 60) * centAngle);
    p.setPen(colorBetween(Qt::white, Qt::darkRed, intensity_));
  
    int halfKnobWidth = MAX(toInt(radius * 0.02), 1);
    
    QPoint noteX(toInt(halfWidth + radius * cos(note)),
                toInt(radius - radius * sin(note)));
  
    QPoint knobNote(toInt(halfWidth + halfKnobWidth * cos(note)),
                    toInt(height() - halfKnobWidth * sin(note)));
    
    QPoint knobLeft = center - QPoint(halfKnobWidth, 0);
    QPoint knobRight = center + QPoint(halfKnobWidth, 0);
    //QPoint knobRight(width() / 2 + halfKnobWidth, radius);
  
    if(intensity_ > 0.0) {
      p.setBrush(colorBetween(Qt::white, Qt::red, intensity_));
      Q3PointArray points(3);
      points.setPoint(0, noteX);
      points.setPoint(1, knobRight);
      points.setPoint(2, knobLeft);
      p.drawPolygon(points);
    }
  }
  endDrawing();
}

/*
 * Sets the value of the widget.
 *
 * @param v the value to represent (in cents). Accepts values between -60 and +60 inclusive.
 * @param intensity How sure of the note you are
 **/
void TunerWidget::setValue(float v, float i)
{
	if (value_ != v && v >= -60 && v <= 60) {
		value_ = v;
		update();
	}
	if (intensity_ != i && i >= 0 && i <= 1) {
		intensity_ = i;
		update();
	}
}

double TunerWidget::value()
{
  return value_;
}
