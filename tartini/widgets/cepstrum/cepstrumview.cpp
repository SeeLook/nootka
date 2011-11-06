/***************************************************************************
                          cepstrumview.cpp  -  description
                             -------------------
    begin                : May 21 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#include "cepstrumview.h"
#include "cepstrumwidget.h"
#include "gdata.h"
#include "channel.h"
//Added by qt3to4:
#include <QResizeEvent>

CepstrumView::CepstrumView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  gdata->setDoingActiveCepstrum(true);

  Channel *active = gdata->getActiveChannel();
  if(active) {
    active->lock();
    active->processChunk(active->currentChunk());
    active->unlock();
  }

  cepstrumWidget = new CepstrumWidget(this);
  cepstrumWidget->setWhatsThis("Note: The 'MPM + MODIFIED CEPSTRUM' option in the preferences needs to be seleted to see this. This is actually a kind of modified cepstrum. ");
  cepstrumWidget->show();

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate(double)), cepstrumWidget, SLOT(update()));
}

CepstrumView::~CepstrumView()
{
  gdata->setDoingActiveCepstrum(false);
  delete cepstrumWidget;
}

void CepstrumView::resizeEvent(QResizeEvent *)
{
  cepstrumWidget->resize(size());
}
