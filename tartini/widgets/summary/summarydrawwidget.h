/***************************************************************************
                          summarydrawwidget.h  -  description
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

#ifndef SUMMARYDRAWWIDGET_H
#define SUMMARYDRAWWIDGET_H

#include "drawwidget.h"
#include "useful.h"

#include <qpixmap.h>
#include <qevent.h>
#include <qobject.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>

#include <map>

// Forward declarations of classes the h file doesn't need to know specifics about
/*class QPixmap;
class QPaintEvent;
class QMouseEvent;
*/


class SummaryDrawWidget : public DrawWidget {
  Q_OBJECT

  public:
    SummaryDrawWidget(QWidget *parent);
    virtual ~SummaryDrawWidget();

    void paintEvent( QPaintEvent * );

    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void wheelEvent( QWheelEvent *e);

  private:
    QPixmap *buffer;

    bool mouseDown;
    int mouseX, mouseY;
    double clickTime, clickNote;
    double clickCurrentTimeDiff, clickViewBottomDiff;
    double scaler, downScaler;
    int clickMode;
};


#endif
