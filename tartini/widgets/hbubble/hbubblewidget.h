/***************************************************************************
                          hstackwidget.h  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef HBUBBLEWIDGET_H
#define HBUBBLEWIDGET_H

#include "drawwidget.h"
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;

class HBubbleWidget : public DrawWidget {
  Q_OBJECT

  public:
    HBubbleWidget(QWidget *parent);
    virtual ~HBubbleWidget();

    void paintEvent( QPaintEvent * );

    QSize sizeHint() const { return QSize(300, 200); }

  private:
    QPixmap *buffer;
    int historyChunks;
    int numHarmonics;

  public slots:
    void setNumHarmonics(double);
    void setHistoryChunks(double);

  signals:
    void numHarmonicsChanged(double);
    void historyChunksChanged(double);
};


#endif
