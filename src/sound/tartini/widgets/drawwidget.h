/***************************************************************************
                          drawwidget.h  -  description
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
#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <qpainter.h>
#include <qwidget.h>
#include <QGLWidget>
#include <qobject.h>
//Added by qt3to4:
#include <QPixmap>
#include <vector>
#include <map>
#include <QPaintDevice>

#include "useful.h"

#define DRAW_VIEW_NORMAL   0
#define DRAW_VIEW_SUMMARY  1
#define DRAW_VIEW_PRINT    2

class Channel;
class QPixmap;
class ZoomElement;

class DrawWidget : public QWidget {
  Q_OBJECT

public:
  DrawWidget(QWidget *parent, const char* name = 0, Qt::WFlags f = Qt::WDestructiveClose);
  virtual ~DrawWidget();

	//static QColor colorBetween(QColor a, QColor b, double ratio);

  /* Draw the channel onto the painter using the MinMax algorithm or individual samples if the zoom is high enough */
  static void drawChannel(QPaintDevice &pd, Channel *ch, QPainter &p, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType);
  void drawChannelFilled(Channel *ch, QPainter &p, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType);
  void setChannelVerticalView(Channel *ch, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY);
  static void setLineWidth(int width);
  
  void beginDrawing(bool clearBackground_=true);
  void endDrawing(bool drawToScreen_=true);
  void clearBackground();
  void fillBackground(const QColor &color);
  void checkSize();
  void drawToScreen();
  static bool calcZoomElement(Channel *ch, ZoomElement &ze, int baseElement, double baseX);
  void drawArray(float *input, int n, int sampleStep=1, double theZoomY=1.0, double offset=0);
  
protected:
  static int lineWidth;
  static int lineTopHalfWidth;
  static int lineBottomHalfWidth;

  QPixmap *_buffer;
  QPaintDevice *paintDevice;

  QPainter p;
};

#endif
