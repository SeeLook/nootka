/***************************************************************************
                          view.cpp  -  description
                             -------------------
    begin                : Mon Jul 26 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "view.h"

#include "useful.h"
#include "gdata.h"
#include "channel.h"
#include "conversions.h"

View::View()
{
  _currentTime = 1.0; //to force a change in the setCurrentTime call
  setCurrentTime(0.0); //in seconds

  //setViewWidth(10.0); //the total time visible in the view (in seconds)
  //setViewHeight(12.0); //the height visible in the view (in semitones)
  setLogZoomX(1.0);
  setLogZoomY(1.0);
  _pixelWidth = 0; //to force a change in the setPixelWidth call
  setPixelWidth(400);
  _pixelHeight = 0; //to force a change in the setPixelWidth call
  setPixelHeight(350);

  _viewOffset = 0.0; //to force a change in the setViewOffset call
  //setViewOffset(5.0); //the amount of time visible before the current time (in seconds)

  setZoomFactorY(3.2);

  _viewBottom = 0.0;
  //setViewBottom(48.0); //the lowest note visible (in semitones) from C0
  setViewBottom(62.0); //the lowest note visible (in semitones) from C0

  //_zoomX = 0.014286;
  //_zoomY = 0.034286;
	
  //_autoFollow = gdata->settings.getBool("View", "autoFollow");
  _autoFollow = gdata->qsettings->value("View/autoFollow", true).toBool();
  //_backgroundShading = gdata->settings.getBool("View", "backgroundShading");
  _backgroundShading = gdata->qsettings->value("View/backgroundShading", true).toBool();

  needFastUpdate = false;
  needSlowUpdate = false;
  fastUpdateTimer = new QTimer(this);
  slowUpdateTimer = new QTimer(this);
  connect(fastUpdateTimer, SIGNAL(timeout()), this, SLOT(nextFastUpdate()));
  connect(slowUpdateTimer, SIGNAL(timeout()), this, SLOT(nextSlowUpdate()));
  connect(this, SIGNAL(viewChanged()), this, SLOT(newUpdate()));
}

View::~View()
{
  delete fastUpdateTimer;
  delete slowUpdateTimer;
}

//plase call this after a window has been created
void View::init()
{
  setViewOffset(viewWidth()/2.0);
}

void View::doUpdate()
{
  doSlowUpdate();
  doFastUpdate();
}

void View::doSlowUpdate()
{
  //emit onSlowUpdate();
  //needSlowUpdate = true;
  //nextSlowUpdate();
  //newUpdate();
  needSlowUpdate = false;
  if(!slowUpdateTimer->isActive()) {
    slowUpdateTimer->start(gdata->slowUpdateSpeed());
  }
  //emit onSlowUpdate();
  emit onSlowUpdate(_currentTime);
}

void View::doFastUpdate()
{
  //emit viewChanged();
  //emit onFastUpdate();
  //needFastUpdate = true;
  //nextFastUpdate();
  //newUpdate();
  needFastUpdate = false;
  if(!fastUpdateTimer->isActive()) {
    fastUpdateTimer->start(gdata->fastUpdateSpeed());
  }
  //emit onFastUpdate();
  emit onFastUpdate(_currentTime);
}

void View::newUpdate()
{
/*
  if(slowUpdateTimer->isActive()) {
	needSlowUpdate = true;
	if(fastUpdateTimer->isActive()) {
		needFastUpdate = true;
	} else {
		emit onFastUpdate();
		needFastUpdate = false;
		fastUpdateTimer->start(gdata->fastUpdateSpeed());
	}
  } else {
	emit onSlowUpdate();
	emit onFastUpdate();
	needFastUpdate = false;
	needSlowUpdate = false;
	slowUpdateTimer->start(gdata->slowUpdateSpeed());
	fastUpdateTimer->start(gdata->fastUpdateSpeed());
  }
*/
  if(slowUpdateTimer->isActive()) {
	needSlowUpdate = true;
  } else {
  	needSlowUpdate = false;
  	slowUpdateTimer->start(gdata->slowUpdateSpeed());
    //emit onSlowUpdate();
    emit onSlowUpdate(_currentTime);
  }
  if(fastUpdateTimer->isActive()) {
    needFastUpdate = true;
  } else {
    needFastUpdate = false;
    fastUpdateTimer->start(gdata->fastUpdateSpeed());
    //emit onFastUpdate();
    emit onFastUpdate(_currentTime);
  }
}

void View::nextFastUpdate()
{
  if(needFastUpdate) {
  	needFastUpdate = false;
  	fastUpdateTimer->start(gdata->fastUpdateSpeed());
    //emit onFastUpdate();
    emit onFastUpdate(_currentTime);
  }
}

void View::nextSlowUpdate()
{
  if(needSlowUpdate) {
    //needFastUpdate = false;
    needSlowUpdate = false;
    //fastUpdateTimer->start(gdata->fastUpdateSpeed());
    slowUpdateTimer->start(gdata->slowUpdateSpeed());
    //emit onFastUpdate();
    //emit onSlowUpdate();
    emit onSlowUpdate(_currentTime);
  }
}

void View::setCurrentTimeRaw(double x)
{
  if(x != _currentTime) {
    Channel *active = gdata->getActiveChannel();
    if(active)
      x = active->timeAtChunk(active->chunkAtTime(x)); //align time to an integer sample step

    _currentTime = x;
  }
}

void View::setCurrentTime(double x)
{
  if(x != _currentTime) {
    Channel *active = gdata->getActiveChannel();
    if(active)
      x = active->timeAtChunk(active->chunkAtTime(x)); //align time to an integer sample step

    //if (x < leftTime()) x = leftTime();
    //if (x > rightTime()) x = rightTime();
    _currentTime = x;
    emit currentTimeChanged(x);
    emit timeViewRangeChanged(viewLeft(), viewRight());
    emit viewChanged();
  }
}
/*
void View::setViewWidth(double w)
{
  if(w != _viewWidth) {
    _viewWidth = w;
    emit timeViewRangeChanged(viewLeft(), viewRight());
    emit viewWidthChanged(w);
    emit viewChanged();
  }
}
*/
/*
void View::setViewHeight(double h)
{
  if(h != _viewHeight) {
    if(h < 0.0) h=0.0;
    if(h > gdata->topNote()-viewBottom()) h=gdata->topNote()-viewBottom();
    _viewHeight = h;
    emit viewHeightChanged(h);
    emit scrollableYChanged(gdata->topNote()-viewHeight());
    emit viewChanged();
  }
}
*/
void View::setViewOffset(double x)
{
  //printf("setViewOffset = %lf\n", x);
  if(x < 0) x = 0;
  if(x > viewWidth()) x = viewWidth();
  if(x != _viewOffset) {
    _viewOffset = x;
    emit timeViewRangeChanged(viewLeft(), viewRight());
    emit viewChanged();
  }
}

void View::setViewBottomRaw(double y)
{
  //printf("y0 = %f\n", y);
  if(y != _viewBottom) {
    _viewBottom = y;
    //_viewBottom = bound(y, 0.0, gdata->topNote()-viewHeight());
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-y);
    //emit viewChanged();
  }
}

void View::setViewBottom(double y)
{
  if(y != _viewBottom) {
    _viewBottom = y;
    //_viewBottom = bound(y, 0.0, gdata->topNote()-viewHeight());
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-y);
    emit viewChanged();
  }
}


/*
void View::changeViewX(int x)
{
  setCurrentTime(double(x) / stepX());
  emit timeRangeChanged(viewLeft(), viewRight());
  emit viewChanged();
}
*/

// Changes the view without using a step value
void View::changeViewX(double x)
{
  setCurrentTime(x);
  emit viewChanged();
}

void View::changeViewY(double y)
{
  setViewBottom(gdata->topPitch() - viewHeight() - y);
  emit viewChanged();
}

/*
void View::viewZoomInX()
{

  double ratio = sqrt(2.0);
  setViewWidth(viewWidth()/ratio);
  setViewOffset(viewOffset()/ratio);

  _zoomX /= ratio;
  emit timeViewRangeChanged(viewLeft(), viewRight());
  emit zoomXChanged(_zoomX);
  emit viewChanged();
  emit doSlowUpdate();

  //printf("Zoom is now %f, width is %f.\n", _zoomX, viewWidth() / 10);
}

void View::viewZoomOutX()
{
  double ratio = sqrt(2.0);
  setViewWidth(viewWidth()*ratio);
  setViewOffset(viewOffset()*ratio);

  _zoomX *= ratio;
  emit timeViewRangeChanged(viewLeft(), viewRight());
  emit zoomXChanged(_zoomX);
  emit viewChanged();
  emit doSlowUpdate();
  //printf("Zoom is now %f.\n", _zoomX);
}
*/

/*
void View::viewZoomInY()
{
  double ratio = sqrt(2.0);
  double old_height = viewHeight();
  //setViewHeight(old_height/ratio);
  //setViewBottom(viewBottom()+(viewHeight()/2.0)*(1.0-1.0/ratio));
  setViewBottom(viewBottom() + (old_height-viewHeight())/2.0);
  //printf("ZoomInY: viewHeight=%lf, viewBottom=%lf\n", viewHeight(), viewBottom());

  _zoomY /= ratio;

  emit zoomYChanged(_zoomY);
  emit viewChanged();

}

void View::viewZoomOutY()
{
  double ratio = sqrt(2.0);
  double old_height = viewHeight();
  double new_height = old_height * ratio;
  if(new_height > gdata->topNote()) {
    new_height = gdata->topNote();
    ratio = new_height / old_height;
  }
  //setViewBottom(viewBottom()-viewHeight()/2.0);
  //setViewHeight(new_height);
  
  //_viewHeight = new_height;
  
  //setViewBottom(viewBottom()-(viewHeight()/2.0)*(1.0-1.0/ratio));
  //setViewBottom(viewBottom() - (viewHeight()-old_height)/2.0);
  double new_bottom = viewBottom() - (new_height-old_height)/2.0;
  if(new_bottom + new_height > gdata->topNote()) new_bottom = gdata->topNote() - new_height;
  setViewBottom(new_bottom);
  //printf("ZoomOutY: viewHeight=%lf, viewBottom=%lf\n", viewHeight(), viewBottom());

  _zoomY *= ratio;

  emit viewHeightChanged(new_height);
  emit scrollableYChanged(gdata->topNote()-viewHeight());
  emit zoomYChanged(_zoomY);
  emit viewChanged();

}
*/

void View::setPixelHeight(int h)
{
  if(h != _pixelHeight) {
    _pixelHeight = h;
    if(viewHeight() > gdata->topPitch()) {
      setLogZoomY(log(double(_pixelHeight) / gdata->topPitch()));
      emit logZoomYChanged(logZoomY());
    }
    emit scrollableYChanged(gdata->topPitch() - viewHeight());
    emit viewHeightChanged(viewHeight());
/*
    if(viewBottom() + viewHeight() > gdata->topNote()) {
      setViewBottom(gdata->topNote() - viewHeight());
    }
    if(viewBottom() < 0.0) setViewBottom(0.0);
    emit viewBottomChanged(gdata->topNote()-viewHeight()-viewBottom());
*/
  }
}

void View::setPixelWidth(int w)
{
  if(w != _pixelWidth) {
    _pixelWidth = w;
    if(viewWidth() > gdata->totalTime() * 2.0) {
      setLogZoomX(log(double(_pixelWidth) / (gdata->totalTime() * 2.0)));
      emit logZoomXChanged(logZoomX());
    }
    //emit scrollableXChanged(gdata->totalTime() - viewWidth());
    emit viewWidthChanged(viewWidth());
    /*
    if(viewRight() > gdata->rightTime()) {
      setCurrentTime(gdata->rightTime() - viewWidth() + viewOffset());
    }
    if(viewLeft() < gdata->leftTime()) setCurrentTime(gdata->leftTime() + viewOffset());
    */
    //emit viewBottomChanged(gdata->topNote()-viewHeight()-viewBottom());
  }
}

void View::setZoomFactorX(double x)
{
  if(x != logZoomX()) {
    double oldViewWidth = viewWidth();
    setLogZoomX(x);
/*
    if(viewWidth() > gdata->totalTime()*2.0) {
      _logZoomX = log(double(_pixelWidth) / (gdata->totalTime() * 2.0));
    }
*/  
    emit logZoomXChanged(logZoomX());
    //emit scrollableXChanged(gdata->topNote() - viewHeight());
    emit viewWidthChanged(viewWidth());
    
    setViewOffset(viewOffset() * (viewWidth()/oldViewWidth));
    //setViewBottom(prevCenterY - viewHeight()/2.0);
    
    /*
    if(viewRight() > gdata->rightTime()) {
      setCurrentTime(gdata->rightTime() - viewWidth() + viewOffset());
    }
    if(viewLeft() < gdata->leftTime()) setCurrentTime(gdata->leftTime() + viewOffset());
    */
    //emit viewBottomChanged(gdata->topNote()-viewHeight()-viewBottom());
    //printf("1 / zoomX = %lf\n", 1.0 / zoomX());
  }
}

void View::setZoomFactorX(double x, int fixedX)
{
  if(x != logZoomX()) {
    double fixedTime = viewLeft() + zoomX() * double(fixedX);
    double oldViewWidth = viewWidth();
    setLogZoomX(x);
/*    
    if(viewWidth() > gdata->totalTime()*2.0) {
      _logZoomX = log(double(_pixelWidth) / (gdata->totalTime() * 2.0));
    }
*/
    emit logZoomXChanged(logZoomX());
    emit viewWidthChanged(viewWidth());
    
    double ratio = viewWidth() / oldViewWidth;
    setViewOffset(viewOffset() * ratio);
    
    //shift the current time to keep the keep the time fixed at the mouse pointer
    double newTime = viewLeft() + zoomX() * double(fixedX);
    //setCurrentTime(currentTime() - (newTime - fixedTime));
    gdata->updateActiveChunkTime(currentTime() - (newTime - fixedTime));
  }
}

void View::setZoomFactorY(double y)
{
  if(y != logZoomY()) {
    double prevCenterY = viewBottom() + viewHeight()/2.0;
    setLogZoomY(y);
/*
    if(viewHeight() > gdata->topNote()) {
      _logZoomY = log(double(_pixelHeight) / gdata->topNote());
    }
*/
    emit logZoomYChanged(logZoomY());
    emit scrollableYChanged(bound(gdata->topPitch() - viewHeight(), 0.0, gdata->topPitch()));
    emit viewHeightChanged(viewHeight());
    
    setViewBottom(prevCenterY - viewHeight()/2.0);
/*    
    if(viewBottom() + viewHeight() > gdata->topNote()) {
      setViewBottom(gdata->topNote() - viewHeight());
      //emit viewBottomChanged(_viewBottom);
    }
    if(viewBottom() < 0.0) {
      setViewBottom(0.0);
      //emit viewBottomChanged(_viewBottom);
    }
*/
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-viewBottom());
    //printf("viewHeight = %lf\n",  _height / exp(logZoomY()));
  }
/*  
  //calc new height
  double old_height = viewHeight();
  double new_height = exp(logZoomY());
  if(new_height > gdata->topNote()) {
    new_height = gdata->topNote();
    _logZoomY = log(new_height);
  }
  _viewHeight = new_height;
  
  //calc the new bottom note of the view
  double new_bottom = viewBottom() - (new_height-old_height)/2.0;
  if(new_bottom + new_height > gdata->topNote()) new_bottom = gdata->topNote() - new_height;
  else if(new_bottom < 0.0) new_bottom = 0.0;
  setViewBottom(new_bottom);
  //printf("ZoomOutY: viewHeight=%lf, viewBottom=%lf\n", viewHeight(), viewBottom());

  _zoomY *= exp(logZoomY());

  emit viewHeightChanged(new_height);
  emit scrollableYChanged(gdata->topNote()-viewHeight());
  emit zoomYChanged(_zoomY);
  emit viewChanged();
*/
}

void View::setZoomFactorY(double y, int fixedY)
{
  if(y != logZoomY()) {
    double fixedNote = viewBottom() + zoomY() * fixedY;
    //double prevCenterY = viewBottom() + viewHeight()/2.0;
    setLogZoomY(y);
/*
    if(viewHeight() > gdata->topNote()) {
      _logZoomY = log(double(_pixelHeight) / gdata->topNote());
    }
*/
    emit logZoomYChanged(logZoomY());
    emit scrollableYChanged(bound(gdata->topPitch() - viewHeight(), 0.0, gdata->topPitch()));
    emit viewHeightChanged(viewHeight());
    
    double newNote = viewBottom() + zoomY() * fixedY;
    setViewBottom(viewBottom() - (newNote - fixedNote));
    //setViewBottom(prevCenterY - viewHeight()/2.0);
/*    
    if(viewBottom() + viewHeight() > gdata->topNote()) {
      setViewBottom(gdata->topNote() - viewHeight());
    }
    if(viewBottom() < 0.0) {
      setViewBottom(0.0);
    }
*/
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-viewBottom());
  }
}

void View::doAutoFollowing()
{
  if(!autoFollow()) return;
  double time = currentTime();
  // We want the average note value for the time period currentTime to viewRight
 
  //if (!gdata->hasActiveChannel()) return;
  Channel *active = gdata->getActiveChannel();

  if (active == NULL) {
    return;
  }
  

  // We can't autofollow if the file is in real time.
  //if (time == active->totalTime()) return;

  double startTime = time - (viewWidth() / 8.0);
  //double stopTime = viewRight() - (_viewWidth / 2.0);
  double stopTime = time + (viewWidth() / 8.0);
  
  //int startFrame = MAX(int(floor(startTime / active->timePerChunk())), 0);
  int startFrame = bound(toInt(startTime / active->timePerChunk()), 0, active->totalChunks()-1);
  int stopFrame = bound(toInt(stopTime / active->timePerChunk()), 0, active->totalChunks()-1);
  //printf("The channel's size is %d, but I'm going from %d to %d.\n", active->lookup.size(), startFrame, stopFrame);

  float pitch = active->averagePitch(startFrame, stopFrame);

  if (pitch < 0) return; // There were no good notes detected

  float newBottom = pitch - (viewHeight() / 2.0);

  //double diff = fabs(newBottom - _viewBottom);
  //printf("Difference is %f\n", diff);
  //if (diff < 2) return;

  setViewBottom(newBottom);
}

void View::setAutoFollow(bool isChecked)
{
	_autoFollow = isChecked;
	//gdata->settings.setBool("View", "autoFollow", isChecked);
  gdata->qsettings->setValue("View/autoFollow", isChecked);
}

void View::setBackgroundShading(bool isChecked)
{
  _backgroundShading = isChecked;
  //gdata->settings.setBool("View", "backgroundShading", isChecked);
  gdata->qsettings->setValue("View/backgroundShading", isChecked);
  emit onSlowUpdate(_currentTime);
}
