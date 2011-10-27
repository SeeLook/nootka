/***************************************************************************
                          fftview.cpp  -  description
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
#include "fftview.h"
#include "fftwidget.h"
#include "gdata.h"
#include "channel.h"
//Added by qt3to4:
#include <QResizeEvent>

FFTView::FFTView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  gdata->setDoingActiveFFT(true);
  
  Channel *active = gdata->getActiveChannel();
  if(active) {
    active->lock();
    active->processChunk(active->currentChunk());
    active->unlock();
  }

  fftWidget = new FFTWidget(this);
  fftWidget->show();

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), fftWidget, SLOT(update()));
}

FFTView::~FFTView()
{
  gdata->setDoingActiveFFT(false);
  delete fftWidget;
}

void FFTView::resizeEvent(QResizeEvent *)
{
  fftWidget->resize(size());
}
