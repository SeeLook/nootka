/***************************************************************************
                          scorewidget.cpp  -  description
                             -------------------
    begin                : Mar 26 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

#include "scorewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "qfontmetrics.h"
#include "qpen.h"
#include "scoresegmentiterator.h"
#include "musicnotes.h"

ScoreWidget::ScoreWidget(QWidget *parent)
  : DrawWidget(parent)
{
  _scaleY = 10;
  _scaleX = 40;
  _boarderX = 15;
  _boarderY = 15;
  _showBaseClef = true;
  _useFlats = false;
  _showNotes = true;
  _showOpaqueNotes = false;
  _showAllMode = false;
  _fontHeight = 14;
  _pitchOffset = 0; //-12; //display 1 octave down
  _font = QFont("AnyStyle", _fontHeight);
  _fontWidth = QFontMetrics(_font).width('#');
  _lookAhead = 0.5; //half a page look ahead
  _lookAheadGap = 0.2;
  _mouseDown = false;

  int counter = 0;
  for(int j=0; j<=128; j++) {
    flatsLookup[j] = counter;
    if(!isBlackNote(j)) counter++;
    sharpsLookup[j] = counter;
  }
  //make relative to Middle C
  int sharpsMiddleC = sharpsLookup[60];
  int flatsMiddleC = flatsLookup[60];
  for(int j=0; j<=128; j++) {
    sharpsLookup[j] -= sharpsMiddleC;
    flatsLookup[j] -= flatsMiddleC;
  }
}

ScoreWidget::~ScoreWidget()
{
}

/**
  * Draw a crotchet at x, y
  * @param x The lefthand-side x pixel
  * @param y The center y pixel
  * @param stemUp true if the stem is to be drawn upwards, else its drawn downwards
  */
void ScoreWidget::drawNote(int x, int y, StemType stemType, FillType fillType, int numFlicks)
{
  double noteWidth = _scaleY; //toInt((double)_scaleY * 1.2);
  double noteHeight = _scaleY;
  //p.drawEllipse(x - toInt(noteWidth/2), y - toInt(noteHeight/2), toInt(noteWidth), toInt(noteHeight));
  //draw the left hand side of the note on the point
  if(gdata->polish()) p.setRenderHint(QPainter::Antialiasing, true);
  if(fillType == FilledNote) {
    p.setBrush(p.pen().color());
  } else {
    p.setBrush(Qt::NoBrush);
  }
  p.drawEllipse(x, y - toInt(noteHeight/2), toInt(noteWidth), toInt(noteHeight));
  if(gdata->polish()) p.setRenderHint(QPainter::Antialiasing, false);
  if(stemType == StemUp) {
    //int stemX = x + toInt((noteWidth)/2); 
    int stemX = x + toInt(noteWidth); 
    p.drawLine(stemX, y - toInt(3*_scaleY), stemX, y);
    for(int j=0; j<numFlicks; j++) {
      p.drawLine(stemX, y - toInt((3.0 - 0.5*j)*_scaleY), stemX + toInt(noteWidth*0.75), y - toInt((2.5 - 0.5*j)*_scaleY));
    }
  } else if(stemType == StemDown) {
    //int stemX = x - toInt((noteWidth)/2); 
    int stemX = x; 
    p.drawLine(stemX, y + toInt(3*_scaleY), stemX, y);
    for(int j=0; j<numFlicks; j++) {
      p.drawLine(stemX, y + toInt((3.0 - 0.5*j)*_scaleY), stemX + toInt(noteWidth*0.75), y + toInt((2.5 - 0.5*j)*_scaleY));
    }
  }
}

ScoreWidget::NoteType ScoreWidget::getNoteType(double noteLength)
{
  double crotchetLength = 0.4;
  noteLength /= crotchetLength;

       if(noteLength < 0.25- 0.1  ) return DemiSemiQuaver;
  else if(noteLength < 0.5 - 0.125) return SemiQuaver;
  else if(noteLength < 1.0 - 0.25 ) return Quaver;
  else if(noteLength < 2.0 - 0.5  ) return Crotchet;
  else if(noteLength < 4.0 - 1.0  ) return Minum;
  else return SemiBreve;
}

/**
  Draw a single note
  @param x The x pos in pixels
  @param y The y pos of the center of the scale (Middle C)
  @param pitch The midi number of the note (60 == Middle C)
  @param noteLength in seconds
*/
void ScoreWidget::drawNoteAtPitch(int x, int y, int pitch, double noteLength, double volume)
{
  pitch = bound(pitch + _pitchOffset, 0, 128);
  if(!_showAllMode) {
    if(noteLength < 0.2) return; //Don't show really short notes
    if(pitch > 84) return; //Don't show extreame notes
    if(_showBaseClef) {
      if(pitch < 36) return;
    } else {
      if(pitch < 57) return;
    }
  }
  double noteWidth = _scaleY; //toInt((double)_scaleY * 1.2);
  double noteHeight = _scaleY;
  int yOffset;
  int ySteps;
  //int accidentalOffsetX = (_showNotes) ? noteWidth/2 : 0;
  int accidentalOffsetX = (_showNotes) ? 2 : 0;
  int j;
  if(!_showNotes) p.setPen(QPen(Qt::red, 1));
  if(_useFlats) {
    ySteps = flatsLookup[pitch];
    yOffset = toInt(_scaleY * (double)ySteps * 0.5);
    if(isBlackNote(pitch))
      p.drawText(x - accidentalOffsetX - _fontWidth, y - yOffset + _fontHeight/2, "b");
  } else {
    ySteps = sharpsLookup[pitch];
    yOffset = toInt(_scaleY * (double)ySteps * 0.5);
    if(isBlackNote(pitch))
      p.drawText(x - accidentalOffsetX - _fontWidth, y - yOffset + _fontHeight/2, "#");
  }
  //draw any ledger lines
  p.setPen(QPen(Qt::black, 1));
  int legerLeftX = x - toInt(noteWidth/2);
  int legerRightX = x + toInt(noteWidth + noteWidth/2);
  if(ySteps == 0) //Middle C ledger line
    p.drawLine(legerLeftX, y, legerRightX, y);
  for(j=6; j<=ySteps/2; j++) //top ledger lines
    p.drawLine(legerLeftX, y - toInt(_scaleY*j), legerRightX, y - toInt(_scaleY*j));
  for(j = (_showBaseClef) ? -6 : 0; j>=ySteps/2; j--) //bottom ledger lines
    p.drawLine(legerLeftX, y - toInt(_scaleY*j), legerRightX, y - toInt(_scaleY*j));

  NoteType noteType = getNoteType(noteLength);
  QColor noteColor(Qt::black);
  if(!_showOpaqueNotes) noteColor.setAlphaF(volume);
  p.setPen(QPen(noteColor, 2));
  /*if(noteType >= Minum) {
    p.setBrush(Qt::NoBrush);
  } else {
    p.setBrush(noteColor);
  }*/
  if(_showNotes) {
    StemType stemType = StemNone;
    if(noteType <= Minum) stemType = (ySteps < 7) ? StemUp : StemDown;
    int numFlicks = 0;
    if(noteType <= Quaver) numFlicks = 1 + (Quaver - noteType);
    drawNote(x, y-yOffset, stemType, (noteType<=Crotchet) ? FilledNote : EmptyNote, numFlicks);
  } else {
    //p.drawRoundRect(x - toInt(noteWidth/2), y - yOffset - toInt(noteHeight/2), toInt(noteWidth), toInt(noteHeight), 3, 3);
    p.drawRoundRect(x, y - yOffset - toInt(noteHeight/2), toInt(noteLength * _scaleX)-1, toInt(noteHeight), 3, 3);
  }
}

/** Draw a segment of stave lines
  * @param leftX The pixel at the left of the segment
  * @param lineCenterY The y pixel to put middle C on
  * @param widthX The width of the stave segment
  */
void ScoreWidget::drawStaveSegment(int leftX, int lineCenterY, int widthX)
{
  int j;
  p.setPen(QPen(Qt::blue, 1));
  for(j=1; j<=5; j++) 
    p.drawLine(leftX, lineCenterY - toInt(j*_scaleY), leftX+widthX, lineCenterY - toInt(j*_scaleY));

  if(_showBaseClef) {
    p.drawLine(leftX, lineCenterY - toInt(5*_scaleY), leftX, lineCenterY+toInt(5*_scaleY));
    for(j=1; j<=5; j++) 
      p.drawLine(leftX, lineCenterY + toInt(j*_scaleY), leftX+widthX, lineCenterY + toInt(j*_scaleY));
  } else {
    p.drawLine(leftX, lineCenterY - toInt(5*_scaleY), leftX, lineCenterY-toInt(1*_scaleY));
  }
}

/** Draws a segment of the musical score
  * @param ch The channel to get the notes from.
  * @param leftX In pixels (even though a doule)
  * @param lineCenterY In pixels
  * @param leftTime The time is seconds at which is at the lefthand side of the scoreSegment to draw
*/
void ScoreWidget::drawScoreSegment(Channel *ch, double leftX, int lineCenterY, double leftTime, double rightTime)
{
  myassert(ch);
  uint j;

  drawStaveSegment(toInt(leftX), lineCenterY, toInt((rightTime-leftTime)*_scaleX));

  if(ch) {
    p.setPen(QPen(Qt::black, 2));
    ChannelLocker channelLocker(ch);
    //draw the notes
    for(j=0; j<ch->noteData.size(); j++) {
      if(ch->isVisibleNote(j) && ch->isLabelNote(j)) {
        double noteTime = ch->timeAtChunk(ch->noteData[j].startChunk());
        //printf("noteTime = %f\n", noteTime);
        if(between(noteTime, leftTime, rightTime)) {
          //FIXME: avgPitch is quite slow to calc
          //printf("x=%d, y=%d, p=%d\n", left + toInt((noteTime - leftTime)*_scaleX), lineCenterY, toInt(ch->noteData[j].avgPitch()));
          drawNoteAtPitch(toInt(leftX + (noteTime - leftTime)*_scaleX),
                   lineCenterY,
                   toInt(ch->noteData[j].avgPitch()),
                   ch->noteData[j].noteLength(), ch->noteData[j].volume());
        }
      }
    }
  }
}

void ScoreWidget::paintEvent( QPaintEvent * /*pe*/ )
{
  Channel *ch = gdata->getActiveChannel();

  beginDrawing();
  p.setFont(_font);
  p.setBrush(Qt::black);

  ScoreSegmentIterator si(this, ch);
  if(ch) {
    QString pageString;
    p.drawText(_fontWidth, height()-4, pageString.sprintf("Page %d of %d", si.curPage()+1, si.numPages()));
    while(si.next()) {
      drawScoreSegment(ch, si.leftX(), si.lineCenterY(), si.leftTime(), si.rightTime());
      if(si.contains(si.curTime())) {
        int pixelX = toInt(si.leftX() + (si.curTime() - si.leftTime()) * _scaleX);
        p.setPen(QPen(Qt::black));
        p.drawLine(pixelX, si.staveTop(), pixelX, si.staveBottom());
      }
    }
  } else {
    while(si.next()) {
      drawStaveSegment(toInt(si.leftX()), si.lineCenterY(), toInt(si.widthX()));
    }
  }

  //p.setBrush(Qt::NoBrush);
  //p.drawRect(pe->rect());
  endDrawing();
}

void ScoreWidget::mousePressEvent( QMouseEvent *e )
{
  _mouseDown = true;
  Channel *ch = gdata->getActiveChannel();

  ScoreSegmentIterator si(this, ch);
  if(ch) {
    while(si.next()) {
      if(between(e->x(), (int)si.leftX(), (int)(si.leftX()+si.widthX())) &&
         between(e->y(), si.staveTop(), si.staveBottom())) {
        double t = si.leftTime() + ((double)e->x() - si.leftX()) / _scaleX;

        //Find the last note played at time t
        int chunk = ch->chunkAtTime(t);
        AnalysisData *data = ch->dataAtChunk(chunk);
        while (data && data->noteIndex < 0 && chunk > 0) {
          data = ch->dataAtChunk(--chunk);
        }
        if(data && data->noteIndex >= 0) {
            int startChunk = ch->noteData[data->noteIndex].startChunk();
            gdata->updateActiveChunkTime(ch->timeAtChunk(startChunk));
            //gdata->updateActiveChunkTime(t);
            if(gdata->running == STREAM_STOP) {
              gdata->playSound(ch->getParent());
            }
        }
      }
    }
  }/* else {
    while(si.next()) {
      drawStaveSegment(si.leftX(), si.lineCenterY(), toInt(si.widthX()));
    }
  }*/
}

void ScoreWidget::mouseMoveEvent( QMouseEvent * /*e*/ )
{
/*
  if(_mouseDown) {
    Channel *ch = gdata->getActiveChannel();
  
    ScoreSegmentIterator si(this, ch);
    if(ch) {
      while(si.next()) {
        if(between(e->x(), (int)si.leftX(), (int)(si.leftX()+si.widthX())) &&
          between(e->y(), si.staveTop(), si.staveBottom())) {
          double t = si.leftTime() + ((double)e->x() - si.leftX()) / _scaleX;
          gdata->updateActiveChunkTime(t);
        }
      }
    }
  }
*/
}

void ScoreWidget::mouseReleaseEvent( QMouseEvent * /*e*/ )
{
  _mouseDown = false;
}


