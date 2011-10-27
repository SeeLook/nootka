/***************************************************************************
                          freqview.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/



#include "freqview.h"
#include "view.h"
#include "gdata.h"
#include "myscrollbar.h"
#include "freqdrawwidget.h"
#include "freqwidgetGL.h"
#include "amplitudewidget.h"
#include "timeaxis.h"
#include "mainwindow.h"
#include "analysisdata.h"

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
#include <Q3Frame>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

FreqView::FreqView(int viewID_, QWidget *parent)
  : ViewWidget(viewID_, parent)
{
  View *view = gdata->view;

  //setCaption("Frequency View");

  Q3BoxLayout *mainLayout = new Q3VBoxLayout(this);
  mainLayout->setResizeMode(QLayout::SetNoConstraint);
  //QBoxLayout *topLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *bottomLayout = new QHBoxLayout(mainLayout);


  QSplitter *splitter = new QSplitter(Qt::Vertical, this);
  QWidget *topWidget = new QWidget(splitter);
  //topSplitter->setOpaqueResize(true);
  Q3BoxLayout *topLayout = new Q3HBoxLayout(topWidget);
  Q3BoxLayout *topLeftLayout = new Q3VBoxLayout(topLayout);
  
  timeAxis = new TimeAxis(topWidget, gdata->leftTime(), gdata->rightTime(), true);
  timeAxis->setWhatsThis("The time in seconds");
  topLeftLayout->addWidget(timeAxis);
  
/*
  Q3Grid *freqFrame = new Q3Grid(1, topWidget);
  freqFrame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  //freqDrawWidget = new FreqDrawWidget(freqFrame);
  //freqDrawWidget->setFocusPolicy(Qt::NoFocus);
  freqWidgetGL = new FreqWidgetGL(freqFrame);
  freqWidgetGL->setFocusPolicy(Qt::NoFocus);
  topLeftLayout->addWidget(freqFrame);
*/
  QFrame *freqFrame = new QFrame;
  freqFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *freqFrameLayout = new QVBoxLayout;
  freqWidgetGL = new FreqWidgetGL(NULL);
  freqWidgetGL->setWhatsThis("The line represents the musical pitch of the sound. A higher pitch moves up, with note names shown at the left, with octave numbers. "
    "The black vertical line shows the current time. This line's position can be moved. "
    "Pitch-lines are drawn connected only because they change a small amount over a small time step. "
    "Note: This may cause semi-tone note changes appear to be joined. Clicking the background and dragging moves the view. "
    "Clicking a pitch-line selects it. Mouse-Wheel scrolls. Shift-Mouse-Wheel zooms");
  //freqWidgetGL = new FreqDrawWidget(NULL);
  freqFrameLayout->addWidget(freqWidgetGL);
  freqFrameLayout->setMargin(0);
  freqFrameLayout->setSpacing(0);
  freqFrame->setLayout(freqFrameLayout);
  topLeftLayout->addWidget(freqFrame);


  //Q3BoxLayout *topRightLayout = new Q3VBoxLayout(topLayout);
  QVBoxLayout *topRightLayout = new QVBoxLayout();
  
  freqWheelY = new QwtWheel(topWidget);
  freqWheelY->setOrientation(Qt::Vertical);
  freqWheelY->setWheelWidth(14);
  freqWheelY->setRange(1.6, 5.0, 0.001, 1);
  //freqWheelY->setValue(3.2);
  //view->setZoomFactorY(3.2);
  freqWheelY->setValue(view->logZoomY());
  QToolTip::add(freqWheelY, "Zoom pitch contour view vertically");
  topRightLayout->addSpacing(20);
  topRightLayout->addWidget(freqWheelY, 0);
  
  //Create the vertical scrollbar
  //vScrollBar = new MyScrollBar(0, toInt((view->topNote()-view->viewHeight())*view->stepY()), 20, toInt(view->viewHeight()*view->stepY()), toInt(view->viewBottom()*view->stepY()), Qt::Vertical, this);
  //vScrollBar = new MyScrollBar(0, gdata->topNote()-view->viewHeight(), 20, view->viewHeight(), view->viewBottom(), 20, Qt::Vertical, this);
  freqScrollBar = new MyScrollBar(0, gdata->topPitch()-view->viewHeight(), 0.5, view->viewHeight(), 0, 20, Qt::Vertical, topWidget);
  freqScrollBar->setValue(gdata->topPitch()-view->viewHeight()-view->viewBottom());
  topRightLayout->addWidget(freqScrollBar, 4);

  topLayout->addLayout(topRightLayout);

/*
  QPushButton *buttonPlusY = new QPushButton("+", topSplitter);
  buttonPlusY->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonPlusY->setFixedSize(15, 15);
  QToolTip::add(buttonPlusY, "Zoom in vertically");
  QPushButton *buttonMinusY = new QPushButton("-", topSplitter);
  buttonMinusY->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonMinusY->setFixedSize(15, 15);
  QToolTip::add(buttonMinusY, "Zoom out vertically");
*/  
  

  //------------bottom half------------------
  
  QWidget *bottomWidget = new QWidget(splitter);
  Q3BoxLayout *bottomLayout = new Q3VBoxLayout(bottomWidget);
  Q3BoxLayout *bottomTopLayout = new Q3HBoxLayout(bottomLayout);
  
/*
  Q3Grid *amplitudeFrame = new Q3Grid(1, bottomWidget);
  amplitudeFrame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  amplitudeWidget = new AmplitudeWidget(amplitudeFrame);
  bottomTopLayout->addWidget(amplitudeFrame);
*/
  QFrame *amplitudeFrame = new QFrame;
  amplitudeFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *amplitudeFrameLayout = new QVBoxLayout;
  amplitudeWidget = new AmplitudeWidget(NULL);
  amplitudeWidget->setWhatsThis("Shows the volume (or other parameters) at time lined up with the pitch above. Note: You can move the lines to change some thresholds.");
  amplitudeFrameLayout->addWidget(amplitudeWidget);
  amplitudeFrameLayout->setMargin(0);
  amplitudeFrameLayout->setSpacing(0);
  amplitudeFrame->setLayout(amplitudeFrameLayout);
  bottomTopLayout->addWidget(amplitudeFrame);

  Q3BoxLayout *bottomTopRightLayout = new Q3VBoxLayout(bottomTopLayout);
  
  amplitudeWheelY = new QwtWheel(bottomWidget);
  amplitudeWheelY->setOrientation(Qt::Vertical);
  amplitudeWheelY->setWheelWidth(14);
  //amplitudeWheelY->setRange(30, 150, 0.1, 1);
  amplitudeWheelY->setRange(0.2, 1.00, 0.01, 1);
  amplitudeWheelY->setValue(amplitudeWidget->range());
  QToolTip::add(amplitudeWheelY, "Zoom pitch contour view vertically");
  bottomTopRightLayout->addWidget(amplitudeWheelY, 0);
  
  //Create the vertical scrollbar
  //amplitudeScrollBar = new MyScrollBar(0, 150, 20, 80, 0, 20, Qt::Vertical, bottomWidget);
  amplitudeScrollBar = new MyScrollBar(0.0, 1.0-amplitudeWidget->range(), 0.20, amplitudeWidget->range(), 0, 20, Qt::Vertical, bottomWidget);
  bottomTopRightLayout->addWidget(amplitudeScrollBar, 4);
  
  Q3BoxLayout *bottomBottomLayout = new Q3HBoxLayout(bottomLayout);
/*
  QSpinBox *thresholdSpinBox = new QSpinBox(50, 100, 1, bottomWidget, "thresholdSpinBox");
  thresholdSpinBox->setSuffix("%");
  thresholdSpinBox->setFocusPolicy(Qt::NoFocus);
  thresholdSpinBox->setValue(gdata->getActiveIntThreshold());
  QToolTip::add(thresholdSpinBox, "Threshold used in choosing the octave");
  connect(thresholdSpinBox, SIGNAL(valueChanged(int)), gdata, SLOT(resetActiveIntThreshold(int)));
  //connect(thresholdSpinBox, SIGNAL(valueChanged(int)), freqDrawWidget, SLOT(update()));
  connect(thresholdSpinBox, SIGNAL(valueChanged(int)), freqWidgetGL, SLOT(update()));
  connect(gdata, SIGNAL(activeIntThresholdChanged(int)), thresholdSpinBox, SLOT(setValue(int)));
*/
  
  QComboBox *amplitudeModeComboBox = new QComboBox(bottomWidget, "amplitudeTypeModeBox");
  //static const char* amplitudeModeItems[] = { "RMS (Log)", "Peak Amplitude (Log)", "Peak Correlation", "Purity", "Changeness", 0 };
  //amplitudeModeComboBox->insertStrList(amplitudeModeItems);
  //amplitudeModeComboBox->insertStrList(amp_mode_names, NUM_AMP_MODES);
  amplitudeModeComboBox->setWhatsThis("Select different algorithm parameters to view in the bottom pannel");
  int j;
  QStringList s;
  for(j=0; j<NUM_AMP_MODES; j++) s << amp_mode_names[j];
  amplitudeModeComboBox->addItems(s);
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setAmplitudeMode(int)));
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), amplitudeWidget, SLOT(update()));

  QComboBox *pitchContourModeComboBox = new QComboBox(bottomWidget, "pitchContourModeComboBox");
  pitchContourModeComboBox->setWhatsThis("Select whether the Pitch Contour line fades in/out with clarity/loudness of the sound or is a solid dark line");
  //static const char* pitchContourModeItems[] = { "Clarity fading", "Note grouping", 0 };
  //pitchContourModeComboBox->insertStrList(pitchContourModeItems);
  s.clear();
  s << "Clarity fading" << "Note grouping";
  pitchContourModeComboBox->addItems(s);
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setPitchContourMode(int)));
  //connect(pitchContourModeComboBox, SIGNAL(activated(int)), freqDrawWidget, SLOT(update()));
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), freqWidgetGL, SLOT(update()));

  freqWheelX = new QwtWheel(bottomWidget);
  freqWheelX->setOrientation(Qt::Horizontal);
  freqWheelX->setWheelWidth(16);
  freqWheelX->setRange(0.5, 9.0, 0.001, 1);
  freqWheelX->setValue(2.0);
  QToolTip::add(freqWheelX, "Zoom horizontally");
  
  bottomBottomLayout->addStretch(2);
  //bottomBottomLayout->addWidget(thresholdSpinBox, 0);
  //bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(amplitudeModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(pitchContourModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(freqWheelX, 1);
  bottomBottomLayout->addSpacing(16);
 

  
  //Create the horizontal scrollbar and +/- buttons
  //hScrollBar = new MyScrollBar(gdata->leftTime(), gdata->rightTime(), 0.1, view->viewWidth(), view->currentTime(), 10000, Qt::Horizontal, this);
/*  
  QPushButton *buttonPlusX = new QPushButton("+", this);
  buttonPlusX->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonPlusX->setFixedSize(15, 15);
  QToolTip::add(buttonPlusX, "Zoom in horizontal");
  QPushButton *buttonMinusX = new QPushButton("-", this);
  buttonMinusX->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonMinusX->setFixedSize(15, 15);
  QToolTip::add(buttonMinusX, "Zoom out horizontal");
*/

  //Create the resize grip. The thing in the bottom right hand corner
  QSizeGrip *sizeGrip = new QSizeGrip(bottomWidget);
  sizeGrip->setFixedSize(15, 15);
  //sizeGrip->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
  sizeGrip->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, false));
  bottomBottomLayout->addWidget(sizeGrip);
  
  //Actually add all the widgets into the widget layout
  //topLayout->addWidget(timeAxis);
  mainLayout->addWidget(splitter);
  //rightLayout->addWidget(buttonMinusY);
  //rightLayout->addWidget(buttonPlusY);
  //rightLayout->addWidget(vScrollBar);
  //bottomLayout->addWidget(buttonMinusX);
  //bottomLayout->addWidget(buttonPlusX);
  //mainLayout->addWidget(sizeGrip, 1, 1);
      
  //Setup all the signals and slots
  //vertical

  connect(freqScrollBar, SIGNAL(sliderMoved(double)), view, SLOT(changeViewY(double)));
  connect(freqScrollBar, SIGNAL(sliderMoved(double)), view, SLOT(doSlowUpdate()));

  connect(view, SIGNAL(viewBottomChanged(double)), freqScrollBar, SLOT(setValue(double)));
/*  connect(view, SIGNAL(scrollableYChanged(double)), freqScrollBar, SLOT(setMaxValue(double)));
  connect(view, SIGNAL(viewHeightChanged(double)), freqScrollBar, SLOT(setPageStep(double)));
*/
  connect(freqWheelY, SIGNAL(valueChanged(double)), view, SLOT(setZoomFactorY(double)));
  connect(view, SIGNAL(logZoomYChanged(double)), freqWheelY, SLOT(setValue(double)));
  //connect(view, SIGNAL(logZoomYChanged(double)), freqDrawWidget, SLOT(update()));
  
  //horizontal
  connect(freqWheelX, SIGNAL(valueChanged(double)), view, SLOT(setZoomFactorX(double)));
  connect(view, SIGNAL(logZoomXChanged(double)), freqWheelX, SLOT(setValue(double)));
  //connect(view, SIGNAL(logZoomXChanged(double)), view, SLOT(doSlowUpdate()));
  //connect(buttonPlusX, SIGNAL(clicked()), view, SLOT(viewZoomInX()));
  //connect(buttonMinusX, SIGNAL(clicked()), view, SLOT(viewZoomOutX()));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(setRange(double)));
  //connect(amplitudeWidget, SIGNAL(dBRangeChanged(double)), amplitudeScrollBar, SLOT(setPageStep(double)));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(update()));

  connect(amplitudeScrollBar, SIGNAL(sliderMoved(double)), amplitudeWidget, SLOT(setOffset(double)));
  connect(amplitudeScrollBar, SIGNAL(sliderMoved(double)), amplitudeWidget, SLOT(update()));

  connect(amplitudeWidget, SIGNAL(rangeChanged(double)), this, SLOT(setAmplitudeZoom(double)));
  connect(amplitudeWidget, SIGNAL(rangeChanged(double)), amplitudeWheelY, SLOT(setValue(double)));
  connect(amplitudeWidget, SIGNAL(offsetChanged(double)), amplitudeScrollBar, SLOT(setValue(double)));

/*
  connect(hScrollBar, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  connect(hScrollBar, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(gdata, SIGNAL(leftTimeChanged(double)), hScrollBar, SLOT(setMinValue(double)));
  connect(gdata, SIGNAL(rightTimeChanged(double)), hScrollBar, SLOT(setMaxValue(double)));
  connect(view, SIGNAL(currentTimeChanged(double)), hScrollBar, SLOT(setValue(double)));
  connect(view, SIGNAL(viewWidthChanged(double)), hScrollBar, SLOT(setPageStep(double)));
*/
  //make the widgets get updated when the view changes
  //connect(gdata->view, SIGNAL(onSlowUpdate(double)), freqDrawWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), freqWidgetGL, SLOT(update()));
  //connect(gdata->view, SIGNAL(onSlowUpdate(double)), amplitudeWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), amplitudeWidget, SLOT(update()));
  //connect(gdata->view, SIGNAL(onSlowUpdate(double)), timeAxis, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), timeAxis, SLOT(update()));
  connect(gdata->view, SIGNAL(timeViewRangeChanged(double, double)), timeAxis, SLOT(setRange(double, double)));
}

FreqView::~FreqView()
{
  //Qt deletes the child widgets automatically
}

void FreqView::zoomIn()
{
  bool doneIt = false;
  if(gdata->running != STREAM_FORWARD) {
    //if(freqDrawWidget->hasMouse()) {
    if(freqWidgetGL->hasMouse()) {
      //QPoint mousePos = freqDrawWidget->mapFromGlobal(QCursor::pos());
      QPoint mousePos = freqWidgetGL->mapFromGlobal(QCursor::pos());
      gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1, mousePos.x());
      //gdata->view->setZoomFactorY(gdata->view->logZoomY() + 0.1, freqDrawWidget->height() - mousePos.y());
      gdata->view->setZoomFactorY(gdata->view->logZoomY() + 0.1, freqWidgetGL->height() - mousePos.y());
      doneIt = true;
    } else if(amplitudeWidget->hasMouse()) {
      QPoint mousePos = amplitudeWidget->mapFromGlobal(QCursor::pos());
      gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1, mousePos.x());
      doneIt = true;
    }
  }
  if(!doneIt) {
    gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1);
    gdata->view->setZoomFactorY(gdata->view->logZoomY() + 0.1);
    doneIt = true;
  }
}

void FreqView::zoomOut()
{
  gdata->view->setZoomFactorX(gdata->view->logZoomX() - 0.1);
  if(!amplitudeWidget->hasMouse()) {
    gdata->view->setZoomFactorY(gdata->view->logZoomY() - 0.1);
  }
}

void FreqView::setAmplitudeZoom(double newRange)
{
  amplitudeScrollBar->setRange(0.0, 1.0-newRange);
  amplitudeScrollBar->setPageStep(newRange);
}


