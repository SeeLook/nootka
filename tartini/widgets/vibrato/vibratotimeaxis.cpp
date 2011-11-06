/***************************************************************************
                          vibratotimeaxis.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "vibratotimeaxis.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"

VibratoTimeAxis::VibratoTimeAxis(QWidget *parent, int nls)
  : DrawWidget(parent)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));

  noteLabelOffset = nls + 2; // The horizontal space in pixels a note label requires + 2 for the border of the VibratoWidget
  startChunkToUse = -1;
  currentChunkToUse = -1;
  endChunkToUse = -1;
  noteLengthToUse = 0.0;
  prevCurrentChunk = -1;
  prevWindowOffset = -999999;
  zoomFactorX = 2.0;
}

VibratoTimeAxis::~VibratoTimeAxis()
{
}

void VibratoTimeAxis::paintEvent( QPaintEvent * )
{
  beginDrawing(false);

  fillBackground(colorGroup().background());

  doUpdate();

  if (currentChunkToUse >= 0) {
    QFontMetrics fm = p.fontMetrics();
    QString s;
    p.setBrush(Qt::black);
    p.setFont(QFont("AnyStyle", 12));

    int polyLengthInPixels = toInt((endChunkToUse - startChunkToUse) * zoomFactorX);
    float pixelsPerSecond = polyLengthInPixels / noteLengthToUse;
    int notchesDivider = 2;
    double secondsPerNotch = 5;
    int calculationStep = 1;

    // Calculate which notches and labels to draw
    for (int pixelsPerSecondThreshold = 25; ;pixelsPerSecondThreshold *= 2) {
      if (pixelsPerSecond < pixelsPerSecondThreshold) {
        break;
      } else {
        switch (calculationStep) {
          case 1:
            notchesDivider = 5;
            secondsPerNotch /= 5;
            calculationStep = 2;
            break;
          case 2:
            notchesDivider = 2;
            secondsPerNotch = secondsPerNotch;
            calculationStep = 3;
            break;
          case 3:
            notchesDivider = 2;
            secondsPerNotch /= 2;
            calculationStep = 1;
            break;
        }
      }
    }

    // Draw the notches + labels
    for (int i = 0; i < (noteLengthToUse / secondsPerNotch); i++) {
      int x = toInt((((endChunkToUse - startChunkToUse) * zoomFactorX) / noteLengthToUse) * i * secondsPerNotch - windowOffsetToUse);
      if ((x >= 0) && (x < width())) {
        if (i % notchesDivider == 0) {  // Even: bigger notch + label
          p.drawLine(x, height()-6, x, height()-1);
          // The 1.000001 factors in the following statements prevent freaky rounding/floor errors...
          int minutes = intFloor(i*secondsPerNotch*1.000001) / 60;
          int seconds = intFloor(i*secondsPerNotch*1.000001) % 60;
          int thousandthseconds = intFloor(1000*i*secondsPerNotch*1.000001) % 1000;
          if (thousandthseconds == 0) {  // Label: m:ss
            s.sprintf("%1d:%02d", minutes, seconds);
          } else if (thousandthseconds % 100 == 0) {  // Label: m:ss.h
            s.sprintf("%1d:%02d.%01d", minutes, seconds, thousandthseconds / 100);
          } else if (thousandthseconds % 10 == 0) {  // Label: m:ss.hh
            s.sprintf("%1d:%02d.%02d", minutes, seconds, thousandthseconds / 10);
          } else {  // Label: m:ss.hhh
            s.sprintf("%1d:%02d.%03d", minutes, seconds, thousandthseconds);
          }
          p.drawText(x - fm.width(s)/2, 12, s);
          } else {  // Odd: smaller notch
          p.drawLine(x, height()-3, x, height()-1);
        }
      }
    }

    // Draw the horizontal line at the bottom of the axis
    p.drawLine(0, height()-1, width(), height()-1);
  }
  endDrawing();
}

void VibratoTimeAxis::resizeEvent( QResizeEvent * )
{
  // Do forced update on resize
  prevCurrentChunk = -1;
  update();
}

void VibratoTimeAxis::doUpdate()
{
  Channel *active = gdata->getActiveChannel();

  int myStartChunk = -1;
  int myCurrentChunk = -1;
  int myEndChunk = -1;
  double myNoteLength = 0.0;
  int myWindowOffset = -999999;

  if (active) {
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &active->noteData[data->noteIndex];

      myStartChunk = note->startChunk();
      myCurrentChunk = active->chunkAtCurrentTime();
      myEndChunk = note->endChunk();
      myNoteLength = note->noteLength();

      // Calculate windowoffset
      if ((myEndChunk - myStartChunk) * zoomFactorX > width() - 2 * noteLabelOffset) {
        // The vibrato-polyline doesn't fit in the window
        if ((myCurrentChunk - myStartChunk) * zoomFactorX < (width() - 2 * noteLabelOffset)/2) {
          // We're at the left side of the vibrato-polyline
          myWindowOffset = 0 - noteLabelOffset;
        } else if ((myEndChunk - myCurrentChunk) * zoomFactorX < (width() - 2 * noteLabelOffset)/2) {
          // We're at the right side of the vibrato-polyline
          myWindowOffset = toInt((myEndChunk - myStartChunk) * zoomFactorX - width() + noteLabelOffset + 1);
        } else {
          // We're somewhere in the middle of the vibrato-polyline
          myWindowOffset = toInt((myCurrentChunk - myStartChunk) * zoomFactorX - width()/2);
        }
      } else {
        // The vibrato-polyline does fit in the window
        myWindowOffset = 0 - noteLabelOffset;
      }
    }
  }

  if (myCurrentChunk == -1) {
    // No note
    if (prevCurrentChunk == myCurrentChunk) {
      // Still no timeaxis needed, no update needed

    } else {
      // Timeaxis should be erased, update widget

      prevCurrentChunk = -1;
      prevWindowOffset = -999999;

      currentChunkToUse = -1;
    }
  } else {
    // Note
    if (prevWindowOffset == myWindowOffset) {
      // No movement, don't redraw timeaxis

    } else {
      // Position in note to draw has changed, so draw the timeaxis

      prevCurrentChunk = myCurrentChunk;
      prevWindowOffset = myWindowOffset;

      startChunkToUse = myStartChunk;
      currentChunkToUse = myCurrentChunk;
      endChunkToUse = myEndChunk;
      noteLengthToUse = myNoteLength;
      windowOffsetToUse = myWindowOffset;
    }
  }
}

void VibratoTimeAxis::setZoomFactorX(double x)
{
  zoomFactorX = 2*pow10(log2(x/25));

  // Do forced update on changed zoomwheel
  prevWindowOffset = -1;

  update();
}
