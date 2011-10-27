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

#include "hbubblewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

HBubbleWidget::HBubbleWidget(QWidget *parent)
  : DrawWidget(parent)
{

  fprintf(stderr,"Initializing\n");
  historyChunks = 32;
  numHarmonics = 40;

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(update()));
  fprintf(stderr,"Done\n");
}

HBubbleWidget::~HBubbleWidget()
{
} 

void HBubbleWidget::setNumHarmonics(double num)
{
	if (numHarmonics != toInt(num))
	{
		numHarmonics = toInt(num);
 		emit numHarmonicsChanged((double)num);
	}
}


void HBubbleWidget::setHistoryChunks(double num)
{
	if (historyChunks != toInt(num))
	{
		historyChunks = toInt(num);
 		emit historyChunksChanged((double)num);
	}
}

#define min(a,b) (a > b ? b : a)
#define max(a,b) (a > b ? a : b)

void HBubbleWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();
  AnalysisData *data;
  int i, j;
  beginDrawing();

  p.setPen(Qt::NoPen);
  if (active)
  {
    for (j = 0; j < historyChunks; j++)
    {
        data = active->dataAtChunk(active->currentChunk() - historyChunks + j + 1);
        if (data != 0)
        {
	    if (data->harmonicFreq.size() != 0 && data->fundamentalFreq != 0)
	    {
		for (i = 0; i < numHarmonics; i++)
		{
			int radius = toInt((data->harmonicAmpNoCutOff[i]+160.0)/160.0 * (float)height()/numHarmonics/2);
			if (radius > 0)
			{
				float flat_sharp = (data->harmonicFreq[i] / data->fundamentalFreq - (i+1))*10;
				QColor c;
				if (flat_sharp > 0)
					c = colorBetween(qRgb(255,255,255), qRgb(255,0,0),flat_sharp);
				else
					c = colorBetween(qRgb(255,255,255), qRgb(0,0,255),-flat_sharp);
				p.setBrush(colorBetween(gdata->backgroundColor(),c,((j == (historyChunks-1)) ? 1.0 : (float)j/historyChunks*0.5)));
				p.drawEllipse(toInt(width()/8*3+j * (float)width()/8/historyChunks - radius), 
					      toInt(height() -(float) ((i+1) * height())/(numHarmonics+2) - radius), 
					      radius * 2, 
					      radius * 2);
			}	
		}
	    }
      	}	
    }
  } 

  endDrawing();
}
