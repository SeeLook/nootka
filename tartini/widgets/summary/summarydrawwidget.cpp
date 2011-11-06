/***************************************************************************
                          summarydrawwidget.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "summarydrawwidget.h"
#include "gdata.h"
//#include "mycolor.h"
#include "channel.h"

#include <qthread.h>

#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>


SummaryDrawWidget::SummaryDrawWidget(QWidget *parent)
  : DrawWidget(parent)
{
  scaler = 1.0;

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

SummaryDrawWidget::~SummaryDrawWidget()
{
}

void SummaryDrawWidget::paintEvent( QPaintEvent * )
{
  Channel *ch;

  //if((right - left) <= 0) return;
  View *view = gdata->view;
/*  if (view->totalTime() == 0) {
    buffer = new QPixmap(size());
    buffer->fill(myBackgroundColor);
    bitBlt(this, 0, 0, buffer); */
  if(gdata->totalTime() < 0) return;

  double timeRatio = double(width()) / gdata->totalTime();
  double pitchRatio = double(height()) / (gdata->topPitch() / scaler);

  beginDrawing();

  //draw all the channels
  for(int j = 0; j < (int)gdata->channels.size(); j++) {
    ch = gdata->channels.at(j);
    if(!ch->isVisible()) continue;

    //drawChannel(ch, p, view->leftTime(), (view->totalTime() / (double) width()), 0.0f, (double) view->topNote() / (double) height(), DRAW_VIEW_SUMMARY);
    drawChannel(*this, ch, p, gdata->leftTime(), view->currentTime(), (gdata->totalTime() / (double) width()), 0.0f, (double) gdata->topPitch() / (double) height(), DRAW_VIEW_SUMMARY);
  }

  //draw the view rectangle 
  p.setPen(QPen(colorGroup().highlight(), 1));
  p.drawRect(int((gdata->leftTime()+view->viewLeft())*timeRatio), height()-1-int((view->viewTop())*pitchRatio),
             int(view->viewWidth()*timeRatio), int(view->viewHeight()*pitchRatio));

  //draw the current time line
  p.setPen(QPen(colorGroup().foreground(), 1));
  //p.moveTo(int((gdata->leftTime()+view->currentTime())*timeRatio), 0);
  //p.lineTo(int((gdata->leftTime()+view->currentTime())*timeRatio), height()-1);
  p.drawLine(int((gdata->leftTime()+view->currentTime())*timeRatio), 0, 
             int((gdata->leftTime()+view->currentTime())*timeRatio), height()-1);

  endDrawing();
}

void SummaryDrawWidget::mousePressEvent( QMouseEvent *e)
{
  View *view = gdata->view;
  double timeRatio = double(width()) / gdata->totalTime(); // px per second
  double noteRatio = double(height()) / (100.0 / scaler);
  clickMode = 0;
  mouseDown = true;
  mouseX = e->x();
  mouseY = e->y();

  // Work out where the user clicked
  clickTime = e->x()/timeRatio + gdata->leftTime();
  clickNote = (height()-1-e->y())/noteRatio;
  if(clickTime > view->viewLeft() && clickTime < view->viewRight() &&
     clickNote > view->viewBottom() && clickNote < view->viewTop()) {
	  // If they're in the view, they're dragging it.
    clickCurrentTimeDiff = view->currentTime() - clickTime;
    clickViewBottomDiff = view->viewBottom() - clickNote;
    clickMode = 1;
    downScaler = scaler;
  } else {
	  // Otherwise, move the view to where they clicked.
    view->setCurrentTime(clickTime);
    view->setViewBottom(int(myround(clickNote - (view->viewHeight() / 2))));
    clickMode = 1;
  }

}

void SummaryDrawWidget::mouseMoveEvent( QMouseEvent *e )
{
  if(mouseDown && clickMode == 1) {
    View *view = gdata->view;
  	//double newScaler = downScaler * ((double(height()) - e->y()) / (double(height()) - mouseY));
  	//if(newScaler < 1.0) newScaler = 1.0;
  	//if(newScaler > 1000.0) newScaler = 1000.0;
  	//scaler = newScaler;
    double timeRatio = double(width()) / gdata->totalTime();
    double noteRatio = double(height()) / (100.0 / scaler);

    // We should only change the current time if the file is stopped
    if (gdata->running == STREAM_STOP) view->setCurrentTime(e->x()/timeRatio - gdata->leftTime() + clickCurrentTimeDiff);
    double newBottom = (height()-1- e->y())/noteRatio + clickViewBottomDiff;
    if (newBottom < 0) {
      newBottom = 0;
    }
    if ((newBottom + view->viewHeight()) > gdata->topPitch()) {
      newBottom = gdata->topPitch() - view->viewHeight();
    }
    view->setViewBottom(newBottom);

  }
}

void SummaryDrawWidget::mouseReleaseEvent( QMouseEvent * )
{
  mouseDown = false;
}

void SummaryDrawWidget::wheelEvent( QWheelEvent *e)
{
  e->accept();

  if (e->state() == Qt::AltModifier) {
    int horScale = toInt(gdata->totalTime() / width() * (e->delta() / 10));
    gdata->view->setCurrentTime(gdata->view->currentTime() + horScale);
  } else {
    int vertScale = toInt(gdata->topPitch() / height() * (e->delta() / 10));
    gdata->view->setViewBottom(gdata->view->viewBottom() + vertScale);
  }
}


