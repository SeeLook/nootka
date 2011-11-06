/***************************************************************************
                          pianoview.h  -  description
                             -------------------
    begin                : 24 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef PIANOVIEW_H
#define PIANOVIEW_H

#include "viewwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

class PianoWidget;

class PianoView : public ViewWidget {
  Q_OBJECT

  public:
    PianoView(int viewID_, QWidget *parent = 0);
    virtual ~PianoView();

    void resizeEvent(QResizeEvent *);

    QSize sizeHint() const { return QSize(200, 100); }

public slots:
	void changeKey();
  private:
    PianoWidget *pianoWidget;

};


#endif
