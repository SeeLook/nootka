/***************************************************************************
                          volumemeterview.h  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef VOLUMEMETERVIEW_H
#define VOLUMEMETERVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>
//#include "volumemeterwidget.h"
class VolumeMeterWidget;

class VolumeMeterView : public ViewWidget {
  Q_OBJECT

  public:
    VolumeMeterView(int viewID_, QWidget *parent = 0);
    virtual ~VolumeMeterView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(256, 30); }

  private:
    VolumeMeterWidget *volumeMeterWidget;

};


#endif
