/***************************************************************************
                          debugview.cpp  -  description
                             -------------------
    begin                : Wed Feb 22 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "debugview.h"
#include "debugwidget.h"
#include "gdata.h"
//Added by qt3to4:
#include <QResizeEvent>

DebugView::DebugView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  debugWidget = new DebugWidget(this);
  debugWidget->show();

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(viewChanged()), debugWidget, SLOT(update()));
}

DebugView::~DebugView()
{
  delete debugWidget;
}

void DebugView::resizeEvent(QResizeEvent *)
{
  debugWidget->resize(size());
}
