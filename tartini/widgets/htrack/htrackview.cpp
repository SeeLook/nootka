/***************************************************************************
                          htrackview.cpp  -  description
                             -------------------
    begin                : 29 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "htrackview.h"
#include "htrackwidget.h"
#include "gdata.h"

#include <qlayout.h>
#include <qslider.h>
#include <qwt_wheel.h>
#include <qsizegrip.h>
#include <q3grid.h>
#include <qtooltip.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QResizeEvent>

HTrackView::HTrackView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  //setCaption("HTrack view");
  Q3GridLayout *mainLayout = new Q3GridLayout(this, 2, 2);
  mainLayout->setResizeMode(QLayout::SetNoConstraint);
  //QBoxLayout *topLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
  Q3BoxLayout *rightLayout = new Q3VBoxLayout();
  Q3BoxLayout *bottomLayout = new Q3HBoxLayout();

  Q3Grid *frame = new Q3Grid(1, this);
  frame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  //frame->setLineWidth(2);
  //frame->setMidLineWidth(2);
  QWidget *aWidget = new QWidget(frame);
  hTrackWidget = new HTrackWidget(aWidget);
  hTrackWidget->setWhatsThis("Shows a 3D keyboard with the current note coloured. "
    "Vertical columns (or tracks), each representing a harmonic (or component frequency), protrude from the back, and move further away over time. "
    "The height of each track is related to how much energy is at that frequency. "
    "Tracks alternate in colour for better visibility. It can be seen how the hamonics in a note fit into the musical scale.");
  //hTrackWidget->show();

  peakThresholdSlider = new QSlider(0, 100, 10, 5, Qt::Vertical, this);
  QToolTip::add(peakThresholdSlider, "Thresholding of harmonics");
  
  rotateXWheel = new QwtWheel(this);
  rotateXWheel->setWheelWidth(20);
  rotateXWheel->setRange(-180, 180, 0.1, 1);
  QToolTip::add(rotateXWheel, "Rotate piano horizonally");
  
  rotateYWheel = new QwtWheel(this);
  rotateYWheel->setOrientation(Qt::Vertical);
  rotateYWheel->setWheelWidth(20);
  rotateYWheel->setRange(-90, 0, 0.1, 1);
  QToolTip::add(rotateYWheel, "Rotate piano vertically");
  
  distanceWheel = new QwtWheel(this);
  distanceWheel->setOrientation(Qt::Vertical);
  //distanceWheel->setWheelWidth(20);
  distanceWheel->setRange(100, 5000, 10, 20);
  distanceWheel->setTotalAngle(20*360);
  QToolTip::add(distanceWheel, "Move towards/away from piano");
  
  QPushButton *homeButton = new QPushButton("Reset", this, "homebutton");
  QToolTip::add(homeButton, "Return to the original view");
  
  QSizeGrip *sizeGrip = new QSizeGrip(this);
  //sizeGrip->setFixedSize(15, 15);
  
  mainLayout->addWidget(frame, 0, 0);
  mainLayout->addLayout(bottomLayout, 1, 0);
  mainLayout->addLayout(rightLayout, 0, 1);
  rightLayout->addStretch(2);
  rightLayout->addWidget(peakThresholdSlider);
  rightLayout->addStretch(4);
  rightLayout->addWidget(rotateYWheel);
  rightLayout->addSpacing(14);
  rightLayout->addWidget(distanceWheel);
  bottomLayout->addStretch(0);
  bottomLayout->addWidget(homeButton);
  bottomLayout->addSpacing(14);
  bottomLayout->addWidget(rotateXWheel);
  //bottomLayout->addSpacing(20);
  mainLayout->addWidget(sizeGrip, 1, 1);

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), hTrackWidget, SLOT(update()));
  connect(peakThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setPeakThreshold(int)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleVertical(double)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleVerticalChanged(double)), rotateYWheel, SLOT(setValue(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setDistanceAway(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(distanceAwayChanged(double)), distanceWheel, SLOT(setValue(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleHorizontal(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleHorizontalChanged(double)), rotateXWheel, SLOT(setValue(double)));
  connect(homeButton, SIGNAL(clicked()), hTrackWidget, SLOT(home()));
}

HTrackView::~HTrackView()
{
  delete hTrackWidget;
}

void HTrackView::resizeEvent(QResizeEvent *)
{
  hTrackWidget->resize(size());
}

void HTrackView::setPeakThreshold(int value)
{
  hTrackWidget->setPeakThreshold(float(value) / 100.0);
  hTrackWidget->update();
}
