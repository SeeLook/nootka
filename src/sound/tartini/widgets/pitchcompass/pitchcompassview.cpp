/***************************************************************************
                          pitchcompassview.cpp  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "pitchcompassview.h"
#include "pitchcompassdrawwidget.h"
#include "gdata.h"
//Added by qt3to4:
#include <QResizeEvent>

PitchCompassView::PitchCompassView(int viewID_, QWidget *parent, int mode)
  : ViewWidget(viewID_, parent)
{
  //setCaption("Pitch Compass");
  pitchCompassDrawWidget = new PitchCompassDrawWidget(this, "compass", mode);
  connect(gdata->view, SIGNAL(currentTimeChanged(double)), pitchCompassDrawWidget, SLOT(updateCompass(double)));
  pitchCompassDrawWidget->show();
}

PitchCompassView::~PitchCompassView()
{
}

void PitchCompassView::resizeEvent(QResizeEvent *)
{
  pitchCompassDrawWidget->resize(size());
}

void PitchCompassView::changeMode(int mode)
{
  delete pitchCompassDrawWidget;
  pitchCompassDrawWidget = new PitchCompassDrawWidget(this, "compass", mode);
  connect(gdata->view, SIGNAL(currentTimeChanged(double)), pitchCompassDrawWidget, SLOT(updateCompass(double)));
}
