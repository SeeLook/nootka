/***************************************************************************
                          vibratotimeaxis.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOTIMEAXIS_H
#define VIBRATOTIMEAXIS_H

#include "drawwidget.h"

class VibratoTimeAxis : public DrawWidget {
  Q_OBJECT

  public:
    VibratoTimeAxis(QWidget *parent, int nls);
    virtual ~VibratoTimeAxis();

    QSize sizeHint() const { return QSize(100, 20); }

    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );

  private:
    //data goes here
    int noteLabelOffset; // The horizontal space in pixels a note label requires

    int startChunkToUse;
    int currentChunkToUse;
    int endChunkToUse;
    double noteLengthToUse;
    int windowOffsetToUse;

    int prevCurrentChunk;
    int prevWindowOffset;

    double zoomFactorX;

  public slots:
    void doUpdate();
    void setZoomFactorX(double x);
};


#endif
