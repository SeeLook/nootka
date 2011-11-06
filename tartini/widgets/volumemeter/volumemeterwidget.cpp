/***************************************************************************
                          volumemeterwidget.cpp  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
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

#include <vector>

#include "volumemeterwidget.h"
#include "gdata.h"
#include "channel.h"
#include "soundfile.h"
#include "analysisdata.h"
#include "useful.h"

VolumeMeterWidget::VolumeMeterWidget(QWidget *parent)
  : DrawWidget(parent)
{

  //setPaletteBackgroundColor(colorGroup().background());

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(update()));

  // Define the number of labels to use for available places
  //labelNumbers = new labelNumbers( {2, 3, 4, 4, 6, 6} );
  labelNumbers.push_back(1);
  labelNumbers.push_back(2);
  labelNumbers.push_back(3);
  labelNumbers.push_back(3);
  labelNumbers.push_back(3);
  labelNumbers.push_back(6);

  setFontSize(9);
}

VolumeMeterWidget::~VolumeMeterWidget()
{
}

void VolumeMeterWidget::setFontSize(int fontSize)
{
  _fontSize = fontSize;
  _font = QFont("AnyStyle", _fontSize);
}

void VolumeMeterWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();
  //if(!active) { return; printf("failed active\n"); }
  //AnalysisData *analysisData = active->currentLookup();
  //if(!analysisData) { return; printf("failed currentLookup\n"); }

  beginDrawing(false);
  fillBackground(colorGroup().background());
  p.setFont(_font);
  
  // Work out how many labels we can draw
  QFontMetrics fm = p.fontMetrics();
  int labelWidth = fm.width("-60"); // Since we'll have two characters normally
  int halfLabelWidth = labelWidth / 2;
  int stopWidth = fm.width("0dB") / 2;
  int realWidth = width() - stopWidth - halfLabelWidth; // The actual width of the data
  int places = labelNumbers[MIN(realWidth / 20, 6) - 1]; // How many labels we should have
  int pixelStep = toInt(double(realWidth) / double(places)); // How many pixels to jump between labels/markers
  //int stopPixel = halfLabelWidth + places * pixelStep; // The actual pixel to stop drawing on
  //int fontSize = 9;
  //int lineY = height() - fontSize - 8;
  int lineY = height() - _fontSize - 2;

  int labelStep = 10;

  // Determine how many dB we should jump by each time
  switch (places) {
    case 1:
    labelStep = 60;
    break;
    case 2:
    labelStep = 30;
    break;
    case 3:
    labelStep = 20;
    break;
    case 6:
    labelStep = 10;
    break;
    default:
    myassert(false); // This should never happen!
    break;
  }


  // Draw horizontal line
  //p.setPen(colorGroup().foreground());
  p.setPen(Qt::black);
  //p.drawLine(halfLabelWidth, lineY, stopPixel, lineY);
  //p.setFont(QFont("AnyStyle", fontSize));

  int y = height() - (lineY/2)- 1;

  for (int i=0; i < places; i++) {
    int x = i * pixelStep;
    p.drawText(x, y, QString::number(-60 + (i * labelStep)));
  }
  p.drawText(places * pixelStep - 2, y, "0dB");

/*
  // Draw the number markers
  int secondaryMarker = pixelStep / 2;
  for (int j=0; j <= places; j++) {
    int major = j * pixelStep + halfLabelWidth;
    int minor = major + secondaryMarker;
    p.drawLine(major, lineY, major, lineY + 5);
    p.drawLine(minor, lineY, minor, lineY + 3);
  }
*/
  
  QColor colour;
  //if (active != NULL && active->hasAnalysisData()) {
  //printf("%d, %d\n", active->chunkAtCurrentTime(), active->totalChunks());
  //int chunk = active->chunkAtCurrentTime() - 1;
  //printf("%d, %d\n", active->chunkNum(), active->totalChunks());
  float theVal[2];
  if (active != NULL && active->isValidChunk(active->currentChunk())) {
    int chunk = active->currentChunk();
    if(active->getParent()->numChannels() > 1) {
      theVal[0] = active->getParent()->channels(0)->dataAtChunk(chunk)->maxIntensityDB();
      theVal[1] = active->getParent()->channels(1)->dataAtChunk(chunk)->maxIntensityDB();
    } else {
      theVal[0] = theVal[1] = active->dataAtChunk(chunk)->maxIntensityDB();
    }
  } else {
    theVal[0] = theVal[1] = 0.0;
  }
  
  //AnalysisData &analysisData = active->dataAtCurrentTime();
  //int val = toInt(analysisData->maxIntensity * double(width()));
  //double decibels = 20.0 * log10(analysisData.maxIntensity);
  for(int chnl=0; chnl<2; chnl++) {
    //double decibels = 20.0 * log10(theVal[chnl]);
    double decibels = theVal[chnl];
    // We'll show 60 dB
    //int val = toInt(((double(width() - halfLabelWidth) / 60.0) * decibels) + width() - halfLabelWidth);
    int val = toInt((double(realWidth / 60.0) * decibels) + realWidth);

    //**************************************************************
    //int barWidth = 12; // Not right - needs to be based on realWidth
    //int barWidth = 5; // Not right - needs to be based on realWidth
    int barWidth = 5; // Not right - needs to be based on realWidth
    //**************************************************************
    // double step = realWidth / floor(realWidth / 12)



    //for(int j=halfLabelWidth; j<=val; j+=10) {
    for(int j=halfLabelWidth; j<=realWidth; j+=10) {
      if(j < val) {
        //int col = MIN(MAX(j*255/width(), 0), 255);
        //p.fillRect(j-3, 0, 3, height()-1, QColor(col, 255 - col, 0));
        if ( j < (width() * 0.5)) colour = Qt::blue;
        else if ( j < (width() * 0.85)) colour = QColor(255, 120, 60);
        else colour = Qt::red;
      } else {
        colour = QColor(198, 198, 198);
      }
      
      if(chnl == 0)
        p.fillRect(j, 2, barWidth, lineY/2 -3, colour);
      else
        p.fillRect(j, height() - (lineY/2) + 1, barWidth, lineY/2 -3, colour);
      //p.setPen(Qt::black);
      //p.drawRect(j, 0, barWidth, height()-20+1);
    }
    //p.fillRect(0, height()-1-val, width()-1, height()-1, QColor(Qt::red));
  }

  endDrawing();  
/*
    float pos;
    float *data;
    float step = float(width()) / float(buffer->length());
    // int h2 = height() / 2;
    float hf = float(height());
    unsigned int j;
    int cur=0, prev=0;

    int c;
    //float *temp[gdata->process_channels];
    float *temp[2];

    for(c = 0; c < gdata->process_channels; c++)
        temp[c] = (float *)malloc(buffer->length() * sizeof(float));
    buffer->lock();
    for(c = 0; c < gdata->process_channels; c++)
        memcpy(temp[c], buffer->begin(c), buffer->length() * sizeof(float));
    buffer->unlock();


    QPainter p;
    p.begin(this);

    for(c=0; c < gdata->process_channels; c++) {
	data = temp[c]; //buffer->data[c];
	pos = 0.0;
	if(c == 0) p.setPen(QPen(red, 4));
	else p.setPen(QPen(blue, 4));
	prev = height()-1-int(data[0]*hf);
	//p.moveTo(0, height()-1-int(data[0]*hf));
	for(j=0; j<buffer->length(); j++, pos+=step) {
	    //p.lineTo(j, h2-int(interpolate_linear(buffer->length(), data, pos)*h2f));
	    cur = height()-1-int(data[j]*hf);
	    //p.lineTo(j, cur);
	    p.drawLine(int(pos-step), prev, int(pos), cur);
	    prev = cur;
	}
    }
    p.end();
*/
}
