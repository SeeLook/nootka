/***************************************************************************
                          view.h  -  description
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

#ifndef VIEW_H
#define VIEW_H

#include <qobject.h>
#include <qtimer.h>
#include "useful.h"

class Channel;

class View : public QObject
{
  Q_OBJECT

private:
  double _currentTime; /**< in seconds */
  //double _viewWidth; /**< the total time visible in the view (in seconds) */
  double _viewBottom; /**< the lowest note visible */
  //double _viewHeight; /**< the height visible in the view (in semitones) */
  double _viewOffset; /**< the amount of time visible before the current time (in seconds) */

  double _logZoomX; /**< the log of the number of pixels per second */
  double _logZoomY; /**< the log of the number of pixels per semitone */
  int _pixelHeight; /**< the height of the view window (in pixels) */
  int _pixelWidth; /**< the width of the view window (in pixels) */
  
  double _zoomX; /**< How many seconds of data each pixel must represent (on the x axis) */
  double _zoomY; /**< How many semitones each pixel must represent (on the y axis) */
  
  bool _autoFollow; /**< Whether the view should autofollow or not */
  bool _backgroundShading;

  void setLogZoomX(double x) { _logZoomX = x; _zoomX = 1.0 / exp(_logZoomX); }
  void setLogZoomY(double y) { _logZoomY = bound(y, 0.6, 6.0); _zoomY = 1.0 / exp(_logZoomY); }

  QTimer *fastUpdateTimer;
  QTimer *slowUpdateTimer;
  bool needSlowUpdate;
  bool needFastUpdate;

public:
  View();
  virtual ~View();

  void init(); //plase call this after a window has been created



  // Relates to the views

  /** Returns the current time in seconds */
  double currentTime() { return _currentTime; }

  /** Returns how long the view is in seconds */
  //double viewWidth() { return _viewWidth; }
  //double viewWidth() { return double(_pixelWidth) / exp(_logZoomX); }
  double viewWidth() { return double(_pixelWidth) * _zoomX; }

  /** Allows you to specify how many seconds wide the view should be */
  //void setViewWidth(double w);

  /** Returns how many semitones fit in the view */
  //double viewHeight() { return _viewHeight; }
  double viewHeight() { return double(_pixelHeight) / exp(_logZoomY); }

  /** Allows you to specify how many semitones should fit in the view */
  //void setViewHeight(double h);

  /** Returns how many seconds are visible before the current time */
  double viewOffset() { return _viewOffset; }

  /** Allows you to specify how many seconds should be visible before the current time */
  void setViewOffset(double x);

  /** Returns the lowest note visible in the view */
  double viewBottom() { return _viewBottom; }

  /** Allows you to specify the lowest note that should be visible in the view */
  void setViewBottomRaw(double y);
  void setViewBottom(double y);

  double viewLeft() { return currentTime() - viewOffset(); } /**< Returns the time at the left edge of the view */
  double viewRight() { return viewLeft() + viewWidth(); } /**< Returns the time at the right edge of the view */
  //double viewTotalTime() { return viewRight() - viewLeft(); } /**< Returns how long the view is in seconds */
  double viewTotalTime() { return viewWidth(); } /**< Returns how long the view is in seconds */
  double viewTop() { return viewBottom() + viewHeight(); } /**< Returns how many semitones are visible in the view*/
  /** Returns the screen pixel for the time t will be drawn */
  int screenPixelX(double t) { return toInt((t - viewLeft()) / zoomX()); }
  
  // Relates to where the files start/stop
  
  /** Returns the x zoom factor in seconds per pixel */
  //double zoomX() { return 1.0 / exp(_logZoomX); }
  double zoomX() { return _zoomX; }
  /** Returns the y zoom factor in semitones per pixel */
  //double zoomY() { return 1.0 / exp(_logZoomY); }
  double zoomY() { return _zoomY; }
  
  double logZoomX() { return _logZoomX; }
  double logZoomY() { return _logZoomY; }

  void setPixelHeight(int h);
  void setPixelWidth(int w);
  void setLogZoomYRaw(double y) { setLogZoomY(y); }
    
public slots:
  void doUpdate();
  void doSlowUpdate();
  void doFastUpdate();
  void newUpdate();
  void nextFastUpdate();
  void nextSlowUpdate();
  void setCurrentTimeRaw(double x);
  void setCurrentTime(double x);
  //void changeViewX(int x);
  void changeViewX(double x);
  void changeViewY(double y);
  //void viewZoomInX();
  //void viewZoomOutX();
  //void viewZoomInY();
  //void viewZoomOutY();
  void setZoomFactorX(double x);
  void setZoomFactorX(double x, int fixedX);
  void setZoomFactorY(double y);
  void setZoomFactorY(double y, int fixedY);
  
  void doAutoFollowing();
  bool autoFollow() { return _autoFollow; }
  void setAutoFollow(bool);
  void setBackgroundShading(bool);
  bool backgroundShading() { return _backgroundShading; }
  
signals:
  //void onSlowUpdate();
  void onSlowUpdate(double currentTime_);
  //void onFastUpdate();
  void onFastUpdate(double currentTime_);
  void viewChanged();
  void scrollableYChanged(double x);
  //void currentTimeChanged(int x);
  void currentTimeChanged(double x);
  void viewBottomChanged(double x);
  void viewWidthChanged(double x);
  void viewHeightChanged(double x);
  void viewOffsetChanged(int x);
  void zoomXChanged(double x);
  void zoomYChanged(double x);
  void timeViewRangeChanged(double leftView_, double rightView_);
  void logZoomXChanged(double x);
  void logZoomYChanged(double x);
};

#endif
