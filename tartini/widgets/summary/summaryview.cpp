/***************************************************************************
                          summaryview.cpp  -  description
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

#include "summaryview.h"
//#include "viewwidget.h"
#include "summarydrawwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

SummaryView::SummaryView(int viewID_, QWidget *parent)
 : ViewWidget( viewID_, parent)
{
  //setCaption("Summary View");

  summaryDrawWidget = new SummaryDrawWidget(this);
  summaryDrawWidget->show();
}

SummaryView::~SummaryView()
{
  delete summaryDrawWidget;
}

void SummaryView::resizeEvent(QResizeEvent *)
{
  summaryDrawWidget->resize(size());
}
