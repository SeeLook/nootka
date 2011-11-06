/***************************************************************************
                          debugwidget.h  -  description
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
#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include "drawwidget.h"
//Added by qt3to4:
#include <QPaintEvent>

class DebugWidget : public DrawWidget {
  Q_OBJECT

  public:
    DebugWidget(QWidget *parent);
    virtual ~DebugWidget();

    void paintEvent( QPaintEvent * );
    void printString(const QString &s);

    QSize sizeHint() const { return QSize(250, 500); }

  private:
    int textY;
    int fontHeight;
};


#endif
