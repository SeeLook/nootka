/***************************************************************************
                          cepstrumview.h  -  description
                             -------------------
    begin                : May 21 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef CEPSTRUMVIEW_H
#define CEPSTRUMVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

class CepstrumWidget;

class CepstrumView : public ViewWidget {
  Q_OBJECT

  public:
    CepstrumView(int viewID_, QWidget *parent = 0);
    virtual ~CepstrumView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(400, 128); }

  private:
    CepstrumWidget *cepstrumWidget;

};


#endif
