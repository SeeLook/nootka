/***************************************************************************
                          volumemeterwidget.h  -  description
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
#ifndef VOLUMEMETERWIDGET_H
#define VOLUMEMETERWIDGET_H

#include "drawwidget.h"
//#include "useful.h"

//#include <qpixmap.h>
//#include <qevent.h>
//#include <qobject.h>

#include <vector>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;
class QFont;
//class QPaintEvent;


class VolumeMeterWidget : public DrawWidget {
  Q_OBJECT

  public:
    VolumeMeterWidget(QWidget *parent);
    virtual ~VolumeMeterWidget();

    void paintEvent( QPaintEvent * );

    QSize sizeHint() const { return QSize(256, 30); }
    void setFontSize(int fontSize);
    
  private:
    QPixmap *buffer;
    QFont _font;
    int _fontSize;
    
    std::vector<int> labelNumbers;
};


#endif
