/***************************************************************************
                          tunerview.cpp  -  description
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
#include <qpixmap.h>
#include <qwt_slider.h>
#include <qlayout.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QResizeEvent>
#include <QPaintEvent>

#include "tunerview.h"
#include "tunerwidget.h"
#include "ledindicator.h"
#include "useful.h"
#include "gdata.h"
#include "channel.h"
#include "musicnotes.h"

int LEDLetterLookup[12] = { 2, 2, 3, 3, 4, 5, 5, 6, 6, 0, 0, 1 };

TunerView::TunerView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  //setCaption("Chromatic Tuner");

  Q3GridLayout *layout = new Q3GridLayout(this, 9, 3, 2);
  layout->setResizeMode(QLayout::SetNoConstraint);

  // Tuner widget goes from (0, 0) to (0, 8);
  //tunerWidget = new TunerWidget(this);
  tunerWidget = new VibratoTunerWidget(this);
  layout->addMultiCellWidget(tunerWidget, 0, 0, 0, 8);

  // Slider goes from (2,0) to (2,9)

  //slider = new QwtSlider(this, "slider", Qt::Horizontal, QwtSlider::Bottom, QwtSlider::BgTrough);
#if QWT_VERSION == 0x050000
  slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::Bottom, QwtSlider::BgTrough);
#else
  slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::BottomScale, QwtSlider::BgTrough);
#endif
  slider->setRange(0, 2);
  slider->setReadOnly(false);
  layout->addMultiCellWidget(slider, 1, 1, 0, 8);
  QToolTip::add(slider, "Increase slider to smooth the pitch over a longer time period");

  ledBuffer = new QPixmap();
  leds.push_back(new LEDIndicator(ledBuffer, this, "A"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "B"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "C"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "D"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "E"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "F"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "G"));

  leds.push_back(new LEDIndicator(ledBuffer, this, "#"));


  // Add the leds for note names into the positions (1, 0) to (1, 6)
  for (int n = 0; n < 7; n++) {
    layout->addWidget(leds.at(n), 2, n);
  }

  // (1, 7) is blank
  
  // Add the flat led
  layout->addWidget(leds.at(7), 2, 8);

  layout->setRowStretch( 0, 4 );
  layout->setRowStretch( 1, 1 );
  layout->setRowStretch( 2, 0 ); 
    
  //connect(gdata->view, SIGNAL(onFastUpdate(double)), this, SLOT(update()));
  //connect(gdata, SIGNAL(onChunkUpdate()), tunerWidget, SLOT(doUpdate()));
  connect(gdata, SIGNAL(onChunkUpdate()), this, SLOT(doUpdate()));
  connect(tunerWidget, SIGNAL(ledSet(int, bool)), this, SLOT(setLed(int, bool)));
}

TunerView::~TunerView()
{
  delete slider;
  for (uint i = 0; i < leds.size(); i++) {
    delete leds[i];
  }
  delete ledBuffer;
  delete tunerWidget;
}

void TunerView::resizeEvent(QResizeEvent *)
{
  //tunerWidget->resize(size());
}

void TunerView::resetLeds()
{
 for (uint i = 0; i < leds.size(); i++) {
    leds[i]->setOn(false);
  }
}

void TunerView::slotCurrentTimeChanged(double /*time*/)
{
/*
  Channel *active = gdata->getActiveChannel();

  if (active == NULL || !active->hasAnalysisData()) {
    tunerWidget->setValue(0, 0);
    return;
  }
  ChannelLocker channelLocker(active);

  // To work out note:
  //   * Find the slider's value. This tells us how many seconds to average over.
  //   * Start time is currentTime() - sliderValue.
  //   * Finish time is currentTime().
  //   * Calculate indexes for these times, and use them to call average.
  //

  double sliderVal = slider->value();
  double startTime = time - sliderVal;
  double stopTime = time;

  //int startChunk = MAX(int(floor(startTime / active->timePerChunk())), 0);
  //int stopChunk = MIN(int(floor(stopTime / active->timePerChunk())), active->lookup.size());
  int startChunk = active->chunkAtTime(startTime);
  int stopChunk = active->chunkAtTime(stopTime)+1;

  float pitch;
  if (sliderVal == 0) {
    pitch = active->dataAtCurrentChunk()->pitch;
  } else {
    pitch = active->averagePitch(startChunk, stopChunk);
  }

  float intensity = active->averageMaxCorrelation(startChunk, stopChunk);

  if (pitch <= 0) {
    tunerWidget->setValue(0, 0);
    return;
  }

  int closePitch = toInt(pitch);
  
  // We can work out how many semitones from A the note is
  //int remainder = closeNote % 12;

  resetLeds();
  leds.at(LEDLetterLookup[noteValue(closePitch)])->setOn(true);
  if(isBlackNote(closePitch)) leds.at(leds.size() - 1)->setOn(true);
  
  // Tell the TunerWidget to update itself, given a value in cents
  tunerWidget->setValue(100*(pitch - float(closePitch)), intensity);
  
*/

/* Old code

  int toLight = -1;
  bool sharp = false;
  
  //A A# B C | C# D D# E | F F# G   <-- noteNames
  //0 1  2 3 | 4  5 6  7 | 8 9  10  <-- remainder
  //------------------------------
  //A B C D E F G  #                <-- leds
  //0 1 2 3 4 5 6  7                <-- led index
  //

  // 3 >= remainder <= 7, the LED is (remainder + 1) / 2, and if even put the sharp on
  if (remainder >= 3 && remainder <= 7) {
	  
    toLight = (remainder + 1) / 2;
    if (remainder % 2 == 0) sharp = true;
  } else {
	// the LED is remainder / 2 + 1 or just / 2, and if it's odd put the sharp led on.
    toLight = (remainder / 2) + 1;
	  if (remainder < 3) toLight = remainder / 2;
    if (remainder % 2 == 1) sharp = true;
  }

  leds.at(toLight)->setOn(true);
  if (sharp) leds.at(leds.size() - 1)->setOn(true);
*/
  
}

void TunerView::paintEvent( QPaintEvent* )
{
  //slotCurrentTimeChanged(gdata->view->currentTime());  
}

void TunerView::setLed(int index, bool value)
{
  leds[index]->setOn(value);
}

void TunerView::doUpdate()
{
  Channel *active = gdata->getActiveChannel();
  if (active == NULL || !active->hasAnalysisData()) {
    tunerWidget->doUpdate(0.0);
    return;
  }
  ChannelLocker channelLocker(active);
  double time = gdata->view->currentTime();

  // To work out note:
  //   * Find the slider's value. This tells us how many seconds to average over.
  //   * Start time is currentTime() - sliderValue.
  //   * Finish time is currentTime().
  //   * Calculate indexes for these times, and use them to call average.
  //

  double sliderVal = slider->value();

  double pitch = 0.0;
  if (sliderVal == 0) {
    int chunk = active->currentChunk();
    if(chunk >= active->totalChunks()) chunk = active->totalChunks()-1;
    if(chunk >= 0)
      pitch = active->dataAtChunk(chunk)->pitch;
  } else {
    double startTime = time - sliderVal;
    double stopTime = time;
  
    int startChunk = active->chunkAtTime(startTime);
    int stopChunk = active->chunkAtTime(stopTime)+1;
    pitch = active->averagePitch(startChunk, stopChunk);
  }

  //float intensity = active->averageMaxCorrelation(startChunk, stopChunk);

  tunerWidget->doUpdate(pitch);
}
