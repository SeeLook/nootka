/***************************************************************************
                          htrackwidget.h  -  description
                             -------------------
    begin                : Aug 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef HTRACK_H
#define HTRACK_H

#include <qgl.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>

class Piano3d;

class HTrackWidget : public QGLWidget
{
  Q_OBJECT        // must include this if you use Qt signals/slots

public:
  HTrackWidget(QWidget *parent, const char *name = NULL);
  ~HTrackWidget();
  
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  
  void rotateX(double angle);
  void rotateY(double angle);
  void translate(float x, float y, float z);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);
  
  float peakThreshold() { return _peakThreshold; }
  double viewAngleHorizontal() { return _viewAngleHorizontal; }
  double viewAngleVertical() { return _viewAngleVertical; }
  
public slots:
  void setPeakThreshold(float peakThreshold) { _peakThreshold = peakThreshold; }
  void setViewAngleHorizontal(double angle) {
    if(angle != _viewAngleHorizontal) { _viewAngleHorizontal = angle; emit viewAngleHorizontalChanged(angle); }
  }
  void setViewAngleVertical(double angle) {
    if(angle != _viewAngleVertical) { _viewAngleVertical = angle; emit viewAngleVerticalChanged(angle); }
  }
  void setDistanceAway(double distance) {
    if(distance != _distanceAway) { _distanceAway = distance; emit distanceAwayChanged(distance); }
  }
  void home();
  
signals:
  void distanceAwayChanged(double);
  void viewAngleHorizontalChanged(double);
  void viewAngleVerticalChanged(double);
  
private:
  float _peakThreshold;
  bool mouseDown;
  int mouseX, mouseY;
  float gCurrentMatrix[16];
  double _distanceAway;
  double _viewAngleHorizontal;
  double _viewAngleVertical;
  //GLuint theKeyboard3d;
  Piano3d *piano3d;
  double translateX, translateY;
};

#endif
