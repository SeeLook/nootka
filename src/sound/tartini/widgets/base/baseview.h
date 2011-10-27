/***************************************************************************
                          baseview.h  -  description
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
#ifndef BASEVIEW_H
#define BASEVIEW_H

#include "viewwidget.h"

class BaseWidget;

class BaseView : public ViewWidget {
  Q_OBJECT

  public:
    BaseView(int viewID_, QWidget *parent = 0);
    virtual ~BaseView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(300, 200); }

  private:
    BaseWidget *baseWidget;

};


#endif
