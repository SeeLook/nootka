/***************************************************************************
                          debugview.h  -  description
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
#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

class DebugWidget;

class DebugView : public ViewWidget {
  Q_OBJECT

  public:
    DebugView(int viewID_, QWidget *parent = 0);
    virtual ~DebugView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(250, 500); }

  private:
    DebugWidget *debugWidget;

};


#endif
