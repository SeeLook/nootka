/***************************************************************************
                          paino.h  -  description
                             -------------------
    begin                : 17 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef PAINO_H
#define PAINO_H

#include <qwidget.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QPaintEvent>
#include "drawwidget.h"

class PianoWidget : public DrawWidget
{
public:
  PianoWidget(QWidget *parent);
  ~PianoWidget();

  void paintEvent(QPaintEvent *);
  
  QSize sizeHint() const { return QSize(165, 100); }

  int currentNote() { return _currentNote; }
  void setCurrentNote(int n, float amountPressed = 1.0);
  void setNoNote() { _currentNote = -1; }
  bool isNote() { return _currentNote >= 0; }
  float amountPressed() { return _amountPressed; }
  void setAmountPressed(float amountPressed) { _amountPressed = amountPressed; }
  
  QPixmap *buffer;
  int _currentNote; //-1=no note, 0=C, 1=C#, 2=D ... 11=B
  float _amountPressed;
};

#endif
