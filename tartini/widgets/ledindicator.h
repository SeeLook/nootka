/***************************************************************************
                          ledindicator.h  -  description
                             -------------------
    begin                : Tue Jan 11 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include "drawwidget.h"
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>

class QColor;
class QPainter;
class QPixmap;

class LEDIndicator : public QWidget {
  Q_OBJECT

  public:
    LEDIndicator(QPixmap *buffer, QWidget *parent = 0, const char *name = 0, const QColor &on = Qt::red, const QColor &off = Qt::darkRed);

    virtual ~LEDIndicator();
    
    void setOn(bool on);
    void toggle();

    bool lit();
    
    void paintEvent(QPaintEvent *);

    QSize sizeHint() const { return QSize(15, 25); }

  private:
    QColor on;
    QColor off;
    bool active;
    bool newState;
    QPixmap *buffer;
};

 
#endif
