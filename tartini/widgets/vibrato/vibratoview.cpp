/***************************************************************************
                          vibratoview.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

/** To create a new tartini widget
    1. Copy the base folder (and files)
    2. Rename the base* files to myclass* files, e.g. mv baseview.cpp myclassview.cpp
    3. Change the class names to MyClassView & MyClassWidget (where MyClass is the name of your new class)
    4. In MainWindow.h increment NUM_VIEWS and add to the enum VIEW_MYCLASS
    5. Add a ViewData item to viewData in MainWindow.cpp
    6. Add #include "myclassview.h" near top of MainWindow.cpp
    7. In MainWindow::openView(int viewID) add a case to create your class
    8. Add your files names and folder names into the pitch.pro (HEADERS, SOURCES and MYPATHS respectivly)
    9. Done. Just rebuild the project (regenerating the Makefile)
*/

#include "vibratoview.h"
#include "vibratotunerwidget.h"
#include "vibratospeedwidget.h"
#include "vibratocirclewidget.h"
#include "vibratoperiodwidget.h"
#include "vibratotimeaxis.h"
#include "vibratowidget.h"
#include "ledindicator.h"
#include "timeaxis.h"
#include "gdata.h"
#include "channel.h"

#include <QGridLayout>
#include <QSplitter>
#include <QToolTip>
#include <QSizeGrip>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <qwt_wheel.h>


VibratoView::VibratoView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  int noteLabelOffset = 28;

  setCaption("Vibrato View");

  QGridLayout *mainLayout = new QGridLayout;

  QSplitter *verticalSplitter = new QSplitter(Qt::Vertical);

  // Top half
  // First column: label + tuner + LEDs
  //QGridLayout *tunerLayout = new QGridLayout;

  // The label
/*
  QLabel *tunerLabel = new QLabel;
  tunerLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  tunerLabel->setText("Chromatic tuner");
  tunerLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  tunerLayout->addWidget(tunerLabel, 0, 0, 1, 9);
  tunerLayout->setRowStretch(0, 0);
*/

  // The tuner
/*
  QFrame *tunerFrame = new QFrame;
  tunerFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *tunerFrameLayout = new QVBoxLayout;
  vibratoTunerWidget = new VibratoTunerWidget(0);
  tunerFrameLayout->addWidget(vibratoTunerWidget);
  tunerFrameLayout->setMargin(0);
  tunerFrameLayout->setSpacing(0);
  tunerFrame->setLayout(tunerFrameLayout);
  tunerLayout->addWidget(tunerFrame, 1, 0, 1, 9);
  tunerLayout->setRowStretch(1, 1);

  // The LEDs
  QPixmap *ledBuffer = new QPixmap();
  leds.push_back(new LEDIndicator(ledBuffer, 0, "A"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "B"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "C"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "D"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "E"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "F"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "G"));
  leds.push_back(new LEDIndicator(ledBuffer, 0, "#"));
  // Add the leds for note names into the positions (2, 0) to (2, 6)
  for (int n = 0; n < 7; n++) {
    tunerLayout->addWidget(leds.at(n), 2, n);
  }
  // (2, 7) is blank
  // Add the flat led at (2, 8)
  tunerLayout->addWidget(leds.at(7), 2, 8);
  tunerLayout->setRowStretch(2, 0);

  tunerLayout->setMargin(1);
  tunerLayout->setSpacing(1);

  QWidget *topWidget1 = new QWidget;
  topWidget1->setLayout(tunerLayout);
*/

  // Second column: Label + speedwidget + button
  QGridLayout *speedLayout = new QGridLayout;

  // The label
  QLabel *speedLabel = new QLabel;
  speedLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  speedLabel->setText("Vibrato speed & width");
  speedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  speedLayout->addWidget(speedLabel, 0, 0, 1, 1);
  speedLayout->setRowStretch(0, 0);

  // The speedwidget
  QFrame *speedFrame = new QFrame;
  speedFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *speedFrameLayout = new QVBoxLayout;
  vibratoSpeedWidget = new VibratoSpeedWidget(0);
  vibratoSpeedWidget->setWhatsThis("Indicates the instantaneous speed and peek-to-peek amplitude of the vibrato. Note: 100 cents = 1 semi-tone (even tempered).");
  speedFrameLayout->addWidget(vibratoSpeedWidget);
  speedFrameLayout->setMargin(0);
  speedFrameLayout->setSpacing(0);
  speedFrame->setLayout(speedFrameLayout);
  speedLayout->addWidget(speedFrame, 1, 0, 1, 1);
  speedLayout->setRowStretch(1, 1);

  // The button
/*
  QPushButton *usePronyButton = new QPushButton("Prony");
  usePronyButton->setCheckable(true);
  usePronyButton->setMinimumWidth(30);
  usePronyButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  usePronyButton->setFocusPolicy(Qt::NoFocus);
  usePronyButton->click();
  QToolTip::add(usePronyButton, "Switch between minmax values and Prony values");
  speedLayout->addWidget(usePronyButton, 2, 0, 1, 1);
  speedLayout->setRowStretch(0, 0);
*/
  speedLayout->setMargin(1);
  speedLayout->setSpacing(1);

  QWidget *topWidget2 = new QWidget;
  topWidget2->setLayout(speedLayout);

  // Third column: Label + circlewidget + horizontal slider
  QGridLayout *circleLayout = new QGridLayout;

  // The label
  QLabel *circleLabel = new QLabel;
  circleLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  circleLabel->setText("The Vibrato circle");
  circleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  circleLayout->addWidget(circleLabel, 0, 0, 1, 2);
  circleLayout->setRowStretch(0, 0);

  // The circlewidget
  QFrame *circleFrame = new QFrame;
  circleFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *circleFrameLayout = new QVBoxLayout;
  vibratoCircleWidget = new VibratoCircleWidget(0);
  vibratoCircleWidget->setWhatsThis("Each cycle of your vibrato is represented by a 2D shape. A current cycle produces a circle if it has a perfect sine wave shape. "
    "Going outside the line indicates your phase is ahead of a sine-wave, and inside the line slower. Note: The shape of one cycle is blended into the next.");
  circleFrameLayout->addWidget(vibratoCircleWidget);
  circleFrameLayout->setMargin(0);
  circleFrameLayout->setSpacing(0);
  circleFrame->setLayout(circleFrameLayout);
  //circleLayout->addWidget(circleFrame, 1, 0, 1, 1);
  circleLayout->addWidget(circleFrame, 1, 0, 1, 2);
  circleLayout->setRowStretch(1, 1);

  // The vertical slider (for selecting type)
/*
  QSlider *vibratoCircleTypeSlider = new QSlider(Qt::Vertical);
  vibratoCircleTypeSlider->setRange(1, 5);
  vibratoCircleTypeSlider->setValue(vibratoCircleWidget->getType());
  vibratoCircleTypeSlider->setFocusPolicy(Qt::NoFocus);
  vibratoCircleTypeSlider->setTickPosition(QSlider::TicksRight);
  vibratoCircleTypeSlider->setTickInterval(1);
  vibratoCircleTypeSlider->setInvertedAppearance(true);
  QToolTip::add(vibratoCircleTypeSlider, "Select type of vibrato circle");
  circleLayout->addWidget(vibratoCircleTypeSlider, 1, 1, 1, 1);
  circleLayout->setRowStretch(0, 0);
*/
  // The horizontal slider (for accuracy)
/*
  QSlider *vibratoCircleAccuracySlider = new QSlider(Qt::Horizontal);
  vibratoCircleAccuracySlider->setRange(1, 50);
  vibratoCircleAccuracySlider->setValue(10);
  vibratoCircleAccuracySlider->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(vibratoCircleAccuracySlider, "Set accuracy of vibrato circle");
  circleLayout->addWidget(vibratoCircleAccuracySlider, 2, 0, 1, 1);
  circleLayout->setRowStretch(0, 0);
*/
  circleLayout->setMargin(1);
  circleLayout->setSpacing(1);

  QWidget *topWidget3 = new QWidget;
  topWidget3->setLayout(circleLayout);

  // Fourth column: Label + periodwidget + buttons for periodwidget
  QGridLayout *periodLayout = new QGridLayout;

  // The label
  QLabel *periodLabel = new QLabel;
  periodLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  periodLabel->setText("Vibrato period view");
  periodLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  periodLayout->addWidget(periodLabel, 0, 0, 1, 6);
  periodLayout->setRowStretch(0, 0);

  // The periodwidget
  QFrame *periodFrame = new QFrame;
  periodFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *periodFrameLayout = new QVBoxLayout;
  vibratoPeriodWidget = new VibratoPeriodWidget(0);
  vibratoPeriodWidget->setWhatsThis("A detailed view of the current vibrato period. You can turn on and off some different options with the buttons. ");
  periodFrameLayout->addWidget(vibratoPeriodWidget);
  periodFrameLayout->setMargin(0);
  periodFrameLayout->setSpacing(0);
  periodFrame->setLayout(periodFrameLayout);
  periodLayout->addWidget(periodFrame, 1, 0, 1, 6);
  periodLayout->setRowStretch(1, 1);

  // The buttons
  QPushButton *smoothedPeriodsButton = new QPushButton("SP");
  smoothedPeriodsButton->setCheckable(true);
  smoothedPeriodsButton->setMinimumWidth(30);
  smoothedPeriodsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  smoothedPeriodsButton->setFocusPolicy(Qt::NoFocus);
  smoothedPeriodsButton->setDown(true);
  QToolTip::add(smoothedPeriodsButton, "Use smoothed periods");
  periodLayout->addWidget(smoothedPeriodsButton, 2, 0, 1, 1);

  QPushButton *drawSineReferenceButton = new QPushButton("DSR");
  drawSineReferenceButton->setCheckable(true);
  drawSineReferenceButton->setMinimumWidth(30);
  drawSineReferenceButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  drawSineReferenceButton->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(drawSineReferenceButton, "Draw the reference sinewave");
  periodLayout->addWidget(drawSineReferenceButton, 2, 1, 1, 1);

  QPushButton *sineStyleButton = new QPushButton("SS");
  sineStyleButton->setCheckable(true);
  sineStyleButton->setMinimumWidth(30);
  sineStyleButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  sineStyleButton->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(sineStyleButton, "Use sine style: /\\/ instead of cosine style: \\/");
  periodLayout->addWidget(sineStyleButton, 2, 2, 1, 1);

  QPushButton *drawPrevPeriodsButton = new QPushButton("DPP");
  drawPrevPeriodsButton->setCheckable(true);
  drawPrevPeriodsButton->setMinimumWidth(30);
  drawPrevPeriodsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  drawPrevPeriodsButton->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(drawPrevPeriodsButton, "Draw previous periods");
  periodLayout->addWidget(drawPrevPeriodsButton, 2, 3, 1, 1);

  QPushButton *periodScalingButton = new QPushButton("PS");
  periodScalingButton->setCheckable(true);
  periodScalingButton->setMinimumWidth(30);
  periodScalingButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  periodScalingButton->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(periodScalingButton, "Scale previous periods to the current period");
  periodLayout->addWidget(periodScalingButton, 2, 4, 1, 1);

  QPushButton *drawComparisonButton = new QPushButton("DC");
  drawComparisonButton->setCheckable(true);
  drawComparisonButton->setMinimumWidth(30);
  drawComparisonButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  drawComparisonButton->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(drawComparisonButton, "Draw comparison between period and sinewave");
  periodLayout->addWidget(drawComparisonButton, 2, 5, 1, 1);

  periodLayout->setRowStretch(2, 0);

  periodLayout->setMargin(1);
  periodLayout->setSpacing(1);

  QWidget *topWidget4 = new QWidget;
  topWidget4->setLayout(periodLayout);

  QSplitter *topSplitter = new QSplitter(Qt::Horizontal);
  //topSplitter->addWidget(topWidget1);
  topSplitter->addWidget(topWidget2);
  topSplitter->setStretchFactor(0, 10);
  topSplitter->addWidget(topWidget3);
  topSplitter->setStretchFactor(1, 10);
  topSplitter->addWidget(topWidget4);
  topSplitter->setStretchFactor(2, 5);

  verticalSplitter->addWidget(topSplitter);

  // Bottom half
  QWidget *bottomWidget = new QWidget;
  QGridLayout *bottomLayout = new QGridLayout;

  // The timeaxis
  vibratoTimeAxis = new VibratoTimeAxis(0, noteLabelOffset);

  // The drawing object for displaying vibrato notes
  QFrame *vibratoFrame = new QFrame;
  vibratoFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *vibratoFrameLayout = new QVBoxLayout;
  vibratoWidget = new VibratoWidget(0, noteLabelOffset);
  vibratoWidget->setWhatsThis("Shows the vibrato of the current note. "
    "Grey shading indicates the vibrato envelope. The black line indicates the center pitch. "
    "Other shading indicates half period times. "
    "If there is no vibrato (i.e. no wobbling frequency) it will probably just look a mess. ");
  vibratoFrameLayout->addWidget(vibratoWidget);
  vibratoFrameLayout->setMargin(0);
  vibratoFrameLayout->setSpacing(0);
  vibratoFrame->setLayout(vibratoFrameLayout);

  // The right side: vertical scrollwheel + vertical scrollbar
  QGridLayout *bottomRightLayout = new QGridLayout;

  // The vertical scrollwheel
  QwtWheel *zoomWheelV = new QwtWheel;
  zoomWheelV->setOrientation(Qt::Vertical);
  zoomWheelV->setWheelWidth(14);
  zoomWheelV->setRange(0.3, 25.0, 0.1, 1);
  zoomWheelV->setValue(1.0);
  zoomWheelV->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(zoomWheelV, "Zoom vibrato view vertically");
  bottomRightLayout->addWidget(zoomWheelV, 0, 0, 1, 1);

  // The vertical scrollbar
  QScrollBar *scrollBarV = new QScrollBar(Qt::Vertical);
  scrollBarV->setRange(-250, 250);
  scrollBarV->setValue(0);
  scrollBarV->setPageStep(100);
  QToolTip::add(scrollBarV, "Scroll vibrato view vertically");
  bottomRightLayout->addWidget(scrollBarV, 1, 0, 4, 1);
  bottomRightLayout->setRowStretch(1, 1);

  bottomRightLayout->setMargin(1);
  bottomRightLayout->setSpacing(1);

  // The bottom side: dummy spacer + horizontal scrollwheel + resize grip
  QGridLayout *bottomBottomLayout = new QGridLayout;

  // The dummy spacer
  QLabel *dummyH = new QLabel;
  bottomBottomLayout->addWidget(dummyH, 0, 0, 1, 4);
  bottomBottomLayout->setColumnStretch(0, 1);

  // The horizontal scrollwheel
  QwtWheel *zoomWheelH = new QwtWheel;
  zoomWheelH->setOrientation(Qt::Horizontal);
  zoomWheelH->setWheelWidth(14);
  zoomWheelH->setRange(1, 100, 1, 1);
  zoomWheelH->setValue(25);
  zoomWheelH->setFocusPolicy(Qt::NoFocus);
  QToolTip::add(zoomWheelH, "Zoom vibrato view horizontally");
  bottomBottomLayout->addWidget(zoomWheelH, 0, 4, 1, 1);

  // The resize grip 
  QSizeGrip *sizeGrip = new QSizeGrip(0);
  sizeGrip->setFixedSize(15, 15);
  bottomBottomLayout->addWidget(sizeGrip, 0, 5, 1, 1);

  bottomBottomLayout->setMargin(1);
  bottomBottomLayout->setSpacing(1);

  bottomLayout->addWidget(vibratoTimeAxis, 0, 0, 1, 1);
  bottomLayout->addWidget(vibratoFrame, 1, 0, 1, 1);
  bottomLayout->addLayout(bottomRightLayout, 1, 1, 1, 1);
  bottomLayout->addLayout(bottomBottomLayout, 2, 0, 1, 2);
  bottomLayout->setMargin(1);
  bottomLayout->setSpacing(1);

  bottomWidget->setLayout(bottomLayout);
  verticalSplitter->addWidget(bottomWidget);

  mainLayout->addWidget(verticalSplitter);

  mainLayout->setMargin(1);
  mainLayout->setSpacing(1);

  setLayout(mainLayout);

  // Make signal/slot connections
  //connect(gdata, SIGNAL(onChunkUpdate()), vibratoTunerWidget, SLOT(doUpdate()));
  //connect(vibratoTunerWidget, SIGNAL(ledSet(int, bool)), this, SLOT(setLed(int, bool)));

  connect(gdata, SIGNAL(onChunkUpdate()), vibratoSpeedWidget, SLOT(doUpdate()));
  //connect(usePronyButton, SIGNAL(toggled(bool)), vibratoSpeedWidget, SLOT(setUseProny(bool)));

  connect(gdata, SIGNAL(onChunkUpdate()), vibratoCircleWidget, SLOT(doUpdate()));
  //connect(vibratoCircleAccuracySlider, SIGNAL(valueChanged(int)), vibratoCircleWidget, SLOT(setAccuracy(int)));
  //connect(vibratoCircleTypeSlider, SIGNAL(valueChanged(int)), vibratoCircleWidget, SLOT(setType(int)));

  connect(gdata, SIGNAL(onChunkUpdate()), vibratoPeriodWidget, SLOT(doUpdate()));

  connect(gdata->view, SIGNAL(onFastUpdate(double)), vibratoTimeAxis, SLOT(update()));

  connect(gdata->view, SIGNAL(onFastUpdate(double)), vibratoWidget, SLOT(updateGL()));

  // The vertical scrollbar
  connect(scrollBarV, SIGNAL(valueChanged(int)), vibratoWidget, SLOT(setOffsetY(int)));

  // The zoomwheels
  connect(zoomWheelH, SIGNAL(valueChanged(double)), vibratoWidget, SLOT(setZoomFactorX(double)));
  connect(zoomWheelH, SIGNAL(valueChanged(double)), vibratoTimeAxis, SLOT(setZoomFactorX(double)));
  connect(zoomWheelV, SIGNAL(valueChanged(double)), vibratoWidget, SLOT(setZoomFactorY(double)));

  // The buttons for the period view
  connect(smoothedPeriodsButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setSmoothedPeriods(bool)));
  connect(drawSineReferenceButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setDrawSineReference(bool)));
  connect(sineStyleButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setSineStyle(bool)));
  connect(drawPrevPeriodsButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setDrawPrevPeriods(bool)));
  connect(periodScalingButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setPeriodScaling(bool)));
  connect(drawComparisonButton, SIGNAL(toggled(bool)), vibratoPeriodWidget, SLOT(setDrawComparison(bool)));

}

VibratoView::~VibratoView()
{
  delete vibratoWidget;
}

/*
void VibratoView::setLed(int index, bool value)
{
  leds[index]->setOn(value);
}
*/
