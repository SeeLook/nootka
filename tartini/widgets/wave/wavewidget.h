/***************************************************************************
                          wavewidget.h  -  description
                             -------------------
    begin                : Mon Mar 14 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include "drawwidget.h"
//Added by qt3to4:
#include <QPixmap>
#include <Q3PointArray>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;
class Q3PointArray;

class WaveWidget : public DrawWidget {
  Q_OBJECT

  public:
    WaveWidget(QWidget *parent);
    virtual ~WaveWidget();

    void paintEvent( QPaintEvent * );

    QSize sizeHint() const { return QSize(500, 128); }
    double zoomY() { return _zoomY; }
        
  private:
    QPixmap *buffer;
    Q3PointArray pointArray;
    double _zoomY;

  signals:
    void zoomYChanged(double zoomY_);
    
  public slots:
    void setZoomY(double zoomY_);
};


#endif
