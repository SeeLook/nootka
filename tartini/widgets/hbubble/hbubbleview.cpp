/***************************************************************************
                          hstackview.cpp  -  description
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
#include "hbubbleview.h"
#include "hbubblewidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <q3grid.h>
#include <qwt_wheel.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qspinbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>


HBubbleView::HBubbleView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  setCaption("Harmonic Bubbles");

  Q3BoxLayout *mainLayout = new Q3HBoxLayout(this);

  Q3BoxLayout *leftLayout = new Q3VBoxLayout(mainLayout);
  Q3BoxLayout *rightLayout = new Q3VBoxLayout(mainLayout);
 
  Q3Grid *waveFrame = new Q3Grid(1, this);
  waveFrame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  leftLayout->addWidget(waveFrame);

  hBubbleWidget = new HBubbleWidget(waveFrame);

  Q3BoxLayout *bottomLayout = new Q3HBoxLayout(leftLayout);

  QwtWheel* harmonicsWheel = new QwtWheel(this);
  harmonicsWheel->setOrientation(Qt::Vertical);
  harmonicsWheel->setWheelWidth(14);
  harmonicsWheel->setRange(1, 40, 1, 1);
  harmonicsWheel->setValue(15);
  hBubbleWidget->setNumHarmonics(15);
  QToolTip::add(harmonicsWheel, "Change number of harmonics shown");
  rightLayout->addWidget(harmonicsWheel);
  rightLayout->addStretch(2);
 
  QwtWheel* windowSizeWheel = new QwtWheel(this);
  windowSizeWheel->setOrientation(Qt::Horizontal);
  windowSizeWheel->setWheelWidth(14);
  windowSizeWheel->setRange(32, 1024, 2, 1);
  windowSizeWheel->setValue(128);
  hBubbleWidget->setHistoryChunks(128);
  QToolTip::add(windowSizeWheel, "Change the window size");
  bottomLayout->addWidget(windowSizeWheel);
  bottomLayout->addStretch(2);
  
  connect(harmonicsWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(setNumHarmonics(double)));
  connect(harmonicsWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(update()));

  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(setHistoryChunks(double)));
  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(update()));

}

HBubbleView::~HBubbleView()
{
  delete hBubbleWidget;
}
