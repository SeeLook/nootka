/***************************************************************************
                          volumemeterview.cpp  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "volumemeterview.h"
#include "volumemeterwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

VolumeMeterView::VolumeMeterView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  //setCaption("Volume Meter");

  volumeMeterWidget = new VolumeMeterWidget(this);
  volumeMeterWidget->show();
}

VolumeMeterView::~VolumeMeterView()
{
  delete volumeMeterWidget;
}

void VolumeMeterView::resizeEvent(QResizeEvent *)
{
  volumeMeterWidget->resize(size());
}
