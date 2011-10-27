/***************************************************************************
                          basewidget.h  -  description
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
#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include "drawwidget.h"

class BaseWidget : public DrawWidget {
  Q_OBJECT

  public:
    BaseWidget(QWidget *parent);
    virtual ~BaseWidget();

    void paintEvent( QPaintEvent * );

    QSize sizeHint() const { return QSize(300, 200); }

  private:
    //data goes here
};


#endif
