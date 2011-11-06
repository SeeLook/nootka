/***************************************************************************
                          correlationview.h  -  description
                             -------------------
    begin                : May 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef CORRELATIONVIEW_H
#define CORRELATIONVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

class CorrelationWidget;

class CorrelationView : public ViewWidget {
  Q_OBJECT

  public:
    CorrelationView(int viewID_, QWidget *parent = 0);
    virtual ~CorrelationView();

    //void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(400, 128); }

  private:
    CorrelationWidget *correlationWidget;

};

#endif
