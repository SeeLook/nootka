/***************************************************************************
                          hstackwidget.cpp  -  description
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
#include <QPaintEvent>

#include "hcirclewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

HCircleWidget::HCircleWidget(QWidget *parent)
  : DrawWidget(parent)
{
  //make the widget get updated when the view changes
  zoom = 0.001;
  threshold = -100;
  lowestValue = -110;
  connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

HCircleWidget::~HCircleWidget()
{
} 

void HCircleWidget::setZoom(double num)
{
  if (zoom != num) 
    zoom = num;
}

void HCircleWidget::setThreshold(double num)
{
  if (threshold != num) 
    threshold = num;
}

void HCircleWidget::setLowestValue(double num)
{
  if (lowestValue != num) 
    lowestValue = num;
}

void HCircleWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();
  int numHarmonics = 40;

  beginDrawing();

  double scale;

  QFont *f = new QFont(p.font());
  f->setPointSize(6);
  p.setFont(*f);
  QString s;

  if (threshold > lowestValue)
  {
  	p.setPen(QPen(colorBetween(gdata->backgroundColor(),qRgb(128,128,128),0.3),2));
  	int radius = toInt((double)height()*zoom*(threshold-lowestValue));
  	p.drawEllipse(width()/2-radius,height()/2-radius,2*radius,2*radius);
 	p.drawText(width()/2-radius+5,height()/2,s.sprintf("Threshold"));
  }
  
  //p.setPen(QPen(qRgb(128,128,128),1));
  p.setPen(QPen(QColor(128,128,128),1));

  double step = 10;  

  for (scale = 0; scale > -160; scale -= step)
  {
	if (scale > lowestValue)
	{
	  int radius = toInt((double)height()*zoom*(scale-lowestValue));
	  p.drawEllipse(width()/2-radius,height()/2-radius,2*radius,2*radius);
	  
	  p.drawText(width()/2+radius,height()/2,s.sprintf("%1.1f", scale));
  	}
  }

  
  f = new QFont(p.font());
  f->setPointSize(8);
  p.setFont(*f);

  if (active)
  {
	AnalysisData *data = active->dataAtCurrentChunk();


	if (data)
	{
		int i;
		double octave = data->fundamentalFreq;
		double nextoctave = octave * 2;	
    int dotSize = 6;
    int halfDotSize = dotSize / 2;
		p.setPen(QPen(Qt::black,2));
		for (i = 0; i < numHarmonics; i++)
		{
			if (data->harmonicFreq[i] > nextoctave)
			{
				octave = nextoctave;
				nextoctave = octave * 2;
			}
			if (data->harmonicAmpNoCutOff[i] > MAX(threshold, lowestValue))			
			{
				double angle = (data->harmonicFreq[i] - octave) / (nextoctave - octave) * 2 * PI;
				double size = height()*zoom*(data->harmonicAmpNoCutOff[i]-lowestValue);
				double size1 = height()*zoom*(MAX(threshold, lowestValue)-lowestValue);
				int x1 = toInt(sin(angle)*size1);
				int y1 = toInt(-cos(angle)*size1);
				int x = toInt(sin(angle)*size); 
				int y = toInt(-cos(angle)*size);
				
				p.drawLine(width()/2+x1, height()/2+y1, width()/2+x, height()/2+y);
        p.drawEllipse(width()/2+x-halfDotSize, height()/2+y-halfDotSize, dotSize, dotSize);
				QString s;
				p.drawText(width()/2+x+5, height()/2+y,s.sprintf("%d", i+1));
			}
		}
	}
  }
  	endDrawing();
}
