/***************************************************************************
                          viewwidget.h  -  description
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
#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <qapplication.h>

class ViewWidget : public QWidget
{
  Q_OBJECT

    int viewID;

 public:
  ViewWidget( int viewID_, QWidget *parent = 0, Qt::WFlags f = 0);
  
//  void closeEvent( QCloseEvent * e );
  
// signals:
//  void aboutToClose(int viewID_);
};

#endif
