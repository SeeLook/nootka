/***************************************************************************
                          summaryview.h  -  description
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
#ifndef SUMMARYVIEW_H
#define SUMMARYVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>
//#include "summarydrawwidget.h"
class SummaryDrawWidget;

class SummaryView : public ViewWidget {
  Q_OBJECT

  public:
    SummaryView(int viewID_, QWidget *parent = 0);
    virtual ~SummaryView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(512, 256); }

  private:
    SummaryDrawWidget *summaryDrawWidget;

};


#endif
