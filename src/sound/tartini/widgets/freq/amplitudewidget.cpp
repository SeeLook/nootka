/***************************************************************************
                          amplitudewidget.cpp  -  description
                             -------------------
    begin                : 19 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/


#include "mygl.h"
#include "amplitudewidget.h"
#include "gdata.h"
#include "channel.h"
#include "zoomlookup.h"
#include "qcursor.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3PointArray>
#include <QWheelEvent>
#include <QPaintEvent>
#include "analysisdata.h"
#include "conversions.h"

#include <qpixmap.h>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

AmplitudeWidget::AmplitudeWidget(QWidget * /*parent*/, const char* /*name*/)
//  : DrawWidget(parent, name, Qt::WDestructiveClose)
{
  setMouseTracking(true);
  
  dragMode = DragNone;
  
  //QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
  //setSizePolicy(sizePolicy);
  
  //setFocusPolicy(QWidget::StrongFocus);
  //setDBRange(120.0);
  setRange(0.8);
  setOffset(0.0);
  //lineWidth = 2;
  //lineTopHalfWidth = lineWidth/2 + (lineWidth % 2);
  //lineBottomHalfWidth = lineWidth/2;
  setAttribute(Qt::WA_OpaquePaintEvent);
}

void AmplitudeWidget::initializeGL()
{
  qglClearColor(gdata->backgroundColor());
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  //setLineWidth(1.0f);
}

void AmplitudeWidget::resizeGL(int w, int h)
{
  mygl_resize2d(w, h);
}

AmplitudeWidget::~AmplitudeWidget()
{
}

void AmplitudeWidget::setRange(double newRange)
{
  if(_range != newRange) {
    _range = bound(newRange, 0.0, 1.0);
    setOffset(offset());
    emit rangeChanged(_range);
  }
}

void AmplitudeWidget::setOffset(double newOffset)
{
  newOffset = bound(newOffset, 0.0, maxOffset());
  //if(_offset != newOffset) {
    _offset = newOffset;
    _offsetInv = maxOffset() - _offset;
    emit offsetChanged(_offset);
    emit offsetInvChanged(offsetInv());
  //}
}

void AmplitudeWidget::paintGL()
//void AmplitudeWidget::paintEvent(QPaintEvent *)
{
  glClear(GL_COLOR_BUFFER_BIT);

  setLineWidth(3.0);
  glEnable(GL_LINE_SMOOTH);

  View *view = gdata->view;

  //draw the red/blue background color shading if needed
  if(view->backgroundShading() && gdata->getActiveChannel())
    drawChannelAmplitudeFilledGL(gdata->getActiveChannel());

  setLineWidth(1.0);
  glDisable(GL_LINE_SMOOTH);

  drawVerticalRefLines();

  setLineWidth(3.0);
  glEnable(GL_LINE_SMOOTH);

  //draw all the visible channels
  for (uint i = 0; i < gdata->channels.size(); i++) {
    Channel *ch = gdata->channels.at(i);
    if(!ch->isVisible()) continue;
    //drawChannel(ch, p, view->viewLeft(), view->currentTime(), view->zoomX(), view->viewBottom(), view->zoomY(), DRAW_VIEW_NORMAL);
    drawChannelAmplitudeGL(ch);
  }

  // Draw the current time line
  glDisable(GL_LINE_SMOOTH);
  qglColor(colorGroup().foreground());
  glLineWidth(1.0);
  double curScreenTime = (view->currentTime() - view->viewLeft()) / view->zoomX();
  mygl_line(curScreenTime, 0, curScreenTime, height() - 1);

  // Draw a horizontal line at the current threshold.
  int y;
  double heightRatio = double(height()) / range();

  qglColor(Qt::black);
  y = height() - 1 - toInt((getCurrentThreshold(0) - offsetInv()) * heightRatio);
  mygl_line(0, y, width(), y);

  qglColor(Qt::red);
  y = height() - 1 - toInt((getCurrentThreshold(1) - offsetInv()) * heightRatio);
  mygl_line(0, y, width(), y);

  qglColor(Qt::black);
  renderText(2, height()-3, getCurrentThresholdString());
}

void AmplitudeWidget::drawVerticalRefLines()
{
  //Draw the vertical reference lines
  double timeStep = timeWidth() / double(width()) * 150.0; //time per 150 pixels
  double timeScaleBase = pow10(floor(log10(timeStep))); //round down to the nearest power of 10

  //choose a timeScaleStep which is a multiple of 1, 2 or 5 of timeScaleBase
  int largeFreq;
  if(timeScaleBase * 5.0 < timeStep) { largeFreq = 5; }
  else if (timeScaleBase * 2.0 < timeStep) { largeFreq = 2; }
  else { largeFreq = 2; timeScaleBase /= 2; }

  double timePos = floor(leftTime() / (timeScaleBase*largeFreq)) * (timeScaleBase*largeFreq); //calc the first one just off the left of the screen
  int x, largeCounter=-1;
  double ratio = double(width()) / timeWidth();
  double lTime = leftTime();

  for(; timePos <= rightTime(); timePos += timeScaleBase) {
    if(++largeCounter == largeFreq) {
      largeCounter = 0;
      glColor4ub(25, 125, 170, 128); //draw the darker lines
    } else {
      glColor4ub(25, 125, 170, 64); //draw the lighter lines
	}
    x = toInt((timePos-lTime) * ratio);
    mygl_line(x, 0, x, height()-1);
  }
}

/** This function has the side effect of changing ze
*/
bool AmplitudeWidget::calcZoomElement(ZoomElement &ze, Channel *ch, int baseElement, double baseX)
{
  int startChunk = int(floor(double(baseElement) * baseX));
  myassert(startChunk <= ch->totalChunks());
  int finishChunk = int(floor(double(baseElement+1) * baseX)) + 1;
  myassert(finishChunk <= ch->totalChunks());
  if(startChunk == finishChunk) return false;

  myassert(startChunk < finishChunk);

  int mode = gdata->amplitudeMode();
  std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
    minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessValue(mode));
  myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
  float low = (*amp_mode_func[mode])(a.first->values[mode]);
  float high = (*amp_mode_func[mode])(a.second->values[mode]);
  
  ze.set(low, high, 0, ch->color, NO_NOTE, (startChunk+finishChunk)/2);
  return true;
}

double AmplitudeWidget::calculateElement(AnalysisData *data)
{
  double val = (*amp_mode_func[gdata->amplitudeMode()])(data->values[gdata->amplitudeMode()]);
  return val;
}

double AmplitudeWidget::getCurrentThreshold(int index)
{
  return (*amp_mode_func[gdata->amplitudeMode()])(gdata->ampThreshold(gdata->amplitudeMode(), index));
}

void AmplitudeWidget::setCurrentThreshold(double newThreshold, int index)
{
  newThreshold = bound(newThreshold, 0.0, 1.0);
  if(newThreshold < offsetInv()) setOffset(maxOffset() - newThreshold);
  else if(newThreshold > offsetInv() + range()) setOffset(maxOffset() - (newThreshold - range()));

  gdata->setAmpThreshold(gdata->amplitudeMode(), index, (*amp_mode_inv_func[gdata->amplitudeMode()])(newThreshold));
}

QString AmplitudeWidget::getCurrentThresholdString()
{
  QString thresholdStr;
  thresholdStr.sprintf(amp_display_string[gdata->amplitudeMode()], gdata->ampThreshold(gdata->amplitudeMode(), 0), gdata->ampThreshold(gdata->amplitudeMode(), 1));
  return thresholdStr;
}

void AmplitudeWidget::setLineWidth(float width)
{
  lineWidth = width;
  halfLineWidth = width / 2;
  glLineWidth(lineWidth);
}

//void AmplitudeWidget::drawChannelAmplitude(Channel *ch, QPainter &p)
void AmplitudeWidget::drawChannelAmplitudeGL(Channel *ch)
{
  View *view = gdata->view;
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->amplitudeZoomLookup;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view->zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view->currentTime());
  double leftFrameTime = currentChunk - ((view->currentTime() - view->viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  //double heightRatio = double(height()) / dBRange();
  double heightRatio = double(height()) / range();
  
  //Q3PointArray pointArray(width()*2);
  Array1d<MyGLfloat2d> vertexArray(width()*2);
  int pointIndex = 0;

  if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    //if(baseElement + theWidth > z->size()) z->setSize(baseElement + theWidth);
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      //if(!z->hasValue(baseElement)) {
      if(!ze.isValid()) {
        if(!calcZoomElement(ze, ch, baseElement, baseX)) continue;
      }
      
      //pointArray.setPoint(pointIndex++, n, height() - 1 - toInt((ze.high() - offsetInv()) * heightRatio) - lineTopHalfWidth);
      //pointArray.setPoint(pointIndex++, n, height() - 1 - toInt((ze.low() - offsetInv()) * heightRatio) + lineBottomHalfWidth);
      vertexArray[pointIndex++] = MyGLfloat2d(n, height() - 1 - ((ze.high() - offsetInv()) * heightRatio) - halfLineWidth);
      vertexArray[pointIndex++] = MyGLfloat2d(n, height() - 1 - ((ze.low() - offsetInv()) * heightRatio) + halfLineWidth);
    }
    myassert(pointIndex <= width()*2);
    //p.setPen(ch->color);
    //p.drawLineSegments(pointArray, 0, pointIndex/2);
    qglColor(ch->color);
    glLineWidth(1.0f);
    glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
    glDrawArrays(GL_LINES, 0, pointIndex);
  } else { //baseX <= 1
    float err = 0.0;
    float val = 0.0;
    int intChunk = (int) floor(leftFrameTime); // Integer version of frame time
    double stepSize = 1.0 / baseX; // So we skip some pixels
    float x = 0.0f, y;

    //double start = 0 - stepSize;
    double start = (double(intChunk) - leftFrameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    //int halfSquareSize = squareSize/2;

    double dn = start;
    int totalChunks = ch->totalChunks();
    if(intChunk < 0) { dn += stepSize * -intChunk; intChunk = 0; }
    for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++) {
      AnalysisData *data = ch->dataAtChunk(intChunk);
      myassert(data);
      
      if(!data) continue;
      err = data->correlation();

      val = calculateElement(data);

      //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, val), 1));

      x = dn;
      y = height() - 1 - ((val - offsetInv()) * heightRatio);
      //pointArray.setPoint(pointIndex++, x, y);
      vertexArray[pointIndex++] = MyGLfloat2d(x, y);
    }
    myassert(pointIndex <= width()*2);
    //p.setPen(QPen(ch->color, lineWidth));
    //p.drawPolyline(pointArray, 0, pointIndex);
    qglColor(ch->color);
    glLineWidth(3.0f);
    glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
    glDrawArrays(GL_LINE_STRIP, 0, pointIndex);
  }
}

//void AmplitudeWidget::drawChannelAmplitudeFilled(Channel *ch, QPainter &p)
void AmplitudeWidget::drawChannelAmplitudeFilledGL(Channel *ch)
{
  View *view = gdata->view;
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->amplitudeZoomLookup;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view->zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view->currentTime());
  double leftFrameTime = currentChunk - ((view->currentTime() - view->viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  //double heightRatio = double(height()) / dBRange();
  double heightRatio = double(height()) / range();
  
  //int firstN = n;
  //int lastN = firstN;
  
  //Q3PointArray bottomPoints(width()*2);
  Array1d<MyGLfloat2d> vertexArray(width()*2);
  int pointIndex = 0;
  //int topBottomPointIndex = 0;

  if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    //if(baseElement + theWidth > z->size()) z->setSize(baseElement + theWidth);
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      //if(!z->hasValue(baseElement)) {
      if(!ze.isValid()) {
        if(!calcZoomElement(ze, ch, baseElement, baseX)) continue;
      }
      
      int y = height() - 1 - toInt((ze.high() - offsetInv()) * heightRatio);
      //bottomPoints.setPoint(topBottomPointIndex++, n, y);
      //bottomPoints.setPoint(topBottomPointIndex++, n, height());
      vertexArray[pointIndex++] = MyGLfloat2d(n, y);
      vertexArray[pointIndex++] = MyGLfloat2d(n, height());
      //lastN = n;
    }
    //p.setPen(Qt::NoPen);
    //p.setBrush(gdata->shading1Color());
    //p.drawRect(firstN, 0, lastN, height());
    //p.setPen(gdata->shading2Color());
    //p.drawLineSegments(bottomPoints, 0, topBottomPointIndex/2);
    qglColor(gdata->shading2Color());
    glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
    glDrawArrays(GL_QUAD_STRIP, 0, pointIndex);
  } else { //baseX <= 1
    float err = 0.0;
    float val = 0.0;
    int intChunk = (int) floor(leftFrameTime); // Integer version of frame time
    double stepSize = 1.0 / baseX; // So we skip some pixels
    float x = 0.0f, y;

    //double start = 0 - stepSize;
    double start = (double(intChunk) - leftFrameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    //int halfSquareSize = squareSize/2;

    double dn = start;
    int totalChunks = ch->totalChunks();
    if(intChunk < 0) { dn += stepSize * -intChunk; intChunk = 0; }
    //bottomPoints.setPoint(topBottomPointIndex++, toInt(dn), height());
    //vertexArray[pointIndex++] = MyGLfloat2d((GLfloat)dn, height());
    
    //firstN = toInt(dn);
    for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++) {
      AnalysisData *data = ch->dataAtChunk(intChunk);
      myassert(data);
      
      if(!data) continue;
      err = data->correlation();

      val = calculateElement(data);

      //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, val), 1));

      x = dn;
      y = height() - 1 - ((val - offsetInv()) * heightRatio);
      //bottomPoints.setPoint(topBottomPointIndex++, x, y);
      vertexArray[pointIndex++] = MyGLfloat2d(x, y);
      vertexArray[pointIndex++] = MyGLfloat2d(x, height());
      //lastN = x;
    }
    //bottomPoints.setPoint(topBottomPointIndex, bottomPoints.point(topBottomPointIndex-1).x(), height());
    //if(pointIndex > 0) {
    //  vertexArray[pointIndex] = MyGLfloat2d(vertexArray[pointIndex-1].x, height());
    //  pointIndex++;
    //}
    myassert(pointIndex <= width()*2);
    //p.setPen(Qt::NoPen);
    //p.setBrush(gdata->shading1Color());
    //p.drawRect(firstN, 0, lastN, height());
    //p.setPen(gdata->shading2Color());
    //p.setBrush(gdata->shading2Color());
    //p.drawPolygon(bottomPoints, false, 0, topBottomPointIndex);
    qglColor(gdata->shading2Color());
    glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
    glDrawArrays(GL_QUAD_STRIP, 0, pointIndex);
  }
}

void AmplitudeWidget::mousePressEvent(QMouseEvent *e)
{
  View *view = gdata->view;
  int timeX = toInt(view->viewOffset() / view->zoomX());
  //int pixelAtCurrentNoiseThresholdY = toInt(gdata->noiseThresholdDB() / gdata->dBFloor() / range() * double(height()));
  int pixelAtCurrentNoiseThresholdY;
  dragMode = DragNone;
  
  //Check if user clicked on center bar, to drag it
  if(within(4, e->x(), timeX)) {
    dragMode = DragTimeBar;
    mouseX = e->x();
    return;
  }
  //Check if user clicked on a threshold bar
  for(int j=0; j<2; j++) {
    pixelAtCurrentNoiseThresholdY = height() - 1 - toInt((getCurrentThreshold(j) - offsetInv()) / range() * double(height()));
    if(within(4, e->y(), pixelAtCurrentNoiseThresholdY)) {
      dragMode = DragNoiseThreshold;
      thresholdIndex = j; //remember which thresholdIndex the user clicked
      mouseY = e->y();
      return;
    }
  }
  //Otherwise user has clicked on background
  {
    mouseX = e->x();
    mouseY = e->y();
    dragMode = DragBackground;
    downTime = view->currentTime();
    downOffset = offset();
  }
}

void AmplitudeWidget::mouseMoveEvent(QMouseEvent *e)
{
  View *view = gdata->view;
  int pixelAtCurrentTimeX = toInt(view->viewOffset() / view->zoomX());
  //int pixelAtCurrentNoiseThresholdY = -toInt(gdata->noiseThresholdDB() / dBRange() * double(height()));
  //int pixelAtCurrentNoiseThresholdY = toInt(gdata->noiseThresholdDB() / gdata->dBFloor() / range() * double(height()));
  int pixelAtCurrentNoiseThresholdY;
  
  switch(dragMode) {
  case DragTimeBar:
    {
      int newX = pixelAtCurrentTimeX + (e->x() - mouseX);
      view->setViewOffset(double(newX) * view->zoomX());
      mouseX = e->x();
      view->doSlowUpdate();
    }
    break;
  case DragNoiseThreshold:
    {
      //int newY = pixelAtCurrentNoiseThreshold + (e->y() - mouseY);
      int newY = e->y();  
      /*if(gdata->amplitudeMode() == FREQ_CHANGENESS) {
        gdata->setChangenessThreshold(double(newY) / double(height()));
      } else {
        gdata->setNoiseThresholdDB(double(newY) / double(height()) * gdata->dBFloor() * range());
      }*/
      setCurrentThreshold(double(height() - 1 - newY) / double(height()) * range() + offsetInv(), thresholdIndex);
      mouseY = e->y();
      gdata->view->doSlowUpdate();
    }
    break;
  case DragBackground:
    //view->setViewBottom(downNote - (mouseY - e->y()) * view->zoomY());
    gdata->updateActiveChunkTime(downTime - (e->x() - mouseX) * view->zoomX());
    setOffset(downOffset - (double(e->y() - mouseY) / double(height()) * range()));
    view->doSlowUpdate();
    break;
  case DragNone:
    if(within(4, e->x(), pixelAtCurrentTimeX)) {
  	  setCursor(QCursor(Qt::SplitHCursor));
    } else {
      bool overThreshold = false;
      for(int j=0; j<2; j++) {
        pixelAtCurrentNoiseThresholdY = height() - 1 - toInt((getCurrentThreshold(j) - offsetInv()) / range() * double(height()));
        if(within(4, e->y(), pixelAtCurrentNoiseThresholdY)) overThreshold = true;
      }
      if(overThreshold) setCursor(QCursor(Qt::SplitVCursor));
      else unsetCursor();
    }
  }
}

void AmplitudeWidget::mouseReleaseEvent(QMouseEvent *)
{
  dragMode = DragNone;
}

void AmplitudeWidget::wheelEvent(QWheelEvent * e)
{
    View *view = gdata->view;
    if (!(e->state() & (Qt::ControlModifier | Qt::ShiftModifier))) {
      //view->setViewBottom(view->viewBottom() + double(e->delta()/WHEEL_DELTA) * view->viewHeight() * 0.1);
      if(gdata->running == STREAM_FORWARD) {
        view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3);
      } else {
        if(e->delta() < 0) {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, width()/2);
        } else {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, e->x());
        }
      }
      view->doSlowUpdate();
    }

    e->accept();
}
