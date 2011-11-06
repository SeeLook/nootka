/***************************************************************************
                          viewwidget.cpp  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "viewwidget.h"
#include "mainwindow.h"

ViewWidget::ViewWidget( int viewID_, QWidget * /*parent*/, Qt::WFlags f )
//  : QWidget( parent, name, WDestructiveClose|f )
  //: QWidget( parent, viewData[viewID_].className, Qt::WDestructiveClose|f )
  : QWidget( NULL, Qt::WDestructiveClose|f )
{
  viewID = viewID_;
  setCaption(viewData[viewID].title);
  
  //setBackgroundMode(NoBackground);
  //setAttribute(Qt::WA_OpaquePaintEvent, false);
  setAutoFillBackground(true);
}

/*
void ViewWidget::closeEvent( QCloseEvent * e )
{
    //emit aboutToClose(viewID);
    QWidget::closeEvent(e);
}
*/
