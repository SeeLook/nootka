/***************************************************************************
                          vibratowidget.cpp  -  description
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
#include <QPainter>

#include "vibratowidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "myglfonts.h"
#include "musicnotes.h"

VibratoWidget::VibratoWidget(QWidget *parent, int nls)
  : QGLWidget(parent)
{
  noteLabelOffset = nls; // The horizontal space in pixels a note label requires
  zoomFactorX = 2.0;
  zoomFactorY = 1.0;
  offsetY = 0;
  noteLabelCounter = 0;
  for (int i = 0; i < 100; i++) {
    noteLabels[i].label = QString(8,' ');
    noteLabels[i].y = 0.0f;
  }
  vibratoFont = QFont();
  vibratoFont.setPointSize(9);
}

VibratoWidget::~VibratoWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(verticalPeriodBars, 1);
  glDeleteLists(verticalSeparatorLines, 1);
  glDeleteLists(referenceLines, 1);
  glDeleteLists(pronyWidthBand, 1);
  glDeleteLists(pronyAveragePitch, 1);
  glDeleteLists(vibratoPolyline, 1);
  glDeleteLists(currentWindowBand, 1);
  glDeleteLists(currentTimeLine, 1);
  glDeleteLists(maximaMinimaPoints, 1);
}

void VibratoWidget::initializeGL()
{
  qglClearColor(gdata->backgroundColor());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  verticalPeriodBars = glGenLists(1);
  verticalSeparatorLines = glGenLists(1);
  referenceLines = glGenLists(1);
  pronyWidthBand = glGenLists(1);
  pronyAveragePitch = glGenLists(1);
  vibratoPolyline = glGenLists(1);
  currentWindowBand = glGenLists(1);
  currentTimeLine = glGenLists(1);
  maximaMinimaPoints = glGenLists(1);
}

void VibratoWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  //glMatrixMode(GL_MODELVIEW);

  update();
}

void VibratoWidget::paintGL()
{

  doUpdate();

  // Clear background
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertical bars that indicate the vibrato periods
  glCallList(verticalPeriodBars);

  // Draw the vertical separator lines through the extrema
  glDisable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(verticalSeparatorLines);

  // Draw the horizontal reference lines
  glDisable(GL_LINE_SMOOTH);
  glEnable(GL_LINE_STIPPLE);
  glLineWidth(1.0);
  glLineStipple(1, 64716);  // bitpattern 64716 = 1111110011001100
  glCallList(referenceLines);
  glDisable(GL_LINE_STIPPLE);

  // Draw the light grey band indicating the vibratowidth according to the Prony-algorithm
  glCallList(pronyWidthBand);

  // Draw the average pitch according to the Prony-algorithm
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(pronyAveragePitch);

  // Draw the vibrato-polyline
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);
  glCallList(vibratoPolyline);

  // Draw the light grey band indicating which time is being used in the current window
  glCallList(currentWindowBand);

  // Draw the current timeline
  glDisable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(currentTimeLine);

  // Draw the maxima & minima
  glEnable(GL_POINT_SMOOTH);
  glPointSize(3.0);
  glCallList(maximaMinimaPoints);

  // Draw the note labels
  mygl_font->beginGLtext(width(), height());
  glColor3ub(0,0,0);
  for (int i = 0; i < noteLabelCounter; i++) {
    //renderText(3, noteLabels[i].y - 4, 0, noteLabels[i].label, vibratoFont);
    //renderText(width() - noteLabelOffset + 3, noteLabels[i].y - 4, 0, noteLabels[i].label, vibratoFont);
    mygl_font->drawGLtextRaw(3, noteLabels[i].y - 4, noteLabels[i].label);
    mygl_font->drawGLtextRaw(width() - noteLabelOffset + 3, noteLabels[i].y - 4, noteLabels[i].label);
  }
  mygl_font->endGLtext();

}

void VibratoWidget::doUpdate()
{
  noteLabelCounter = 0;

  makeCurrent();

  glNewList(verticalPeriodBars, GL_COMPILE);
  glEndList();

  glNewList(verticalSeparatorLines, GL_COMPILE);
  glEndList();

  glNewList(referenceLines, GL_COMPILE);
  glEndList();

  glNewList(pronyWidthBand, GL_COMPILE);
  glEndList();

  glNewList(pronyAveragePitch, GL_COMPILE);
  glEndList();

  glNewList(vibratoPolyline, GL_COMPILE);
  glEndList();

  glNewList(currentWindowBand, GL_COMPILE);
  glEndList();

  glNewList(currentTimeLine, GL_COMPILE);
  glEndList();

  glNewList(maximaMinimaPoints, GL_COMPILE);
  glEndList();

  Channel *active = gdata->getActiveChannel();

  if (active) {
    ChannelLocker channelLocker(active);
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);

      const int myStartChunk = note->startChunk();
      const int myEndChunk = note->endChunk();
      const int myCurrentChunk = active->chunkAtCurrentTime();
      const float halfHeight = 0.5 * height();
      const int maximaSize = note->maxima->size();
      const int minimaSize = note->minima->size();
      const float avgPitch = note->avgPitch();
      const int framesPerChunk = active->framesPerChunk();
      const float zoomFactorYx100 = zoomFactorY * 100;

      float windowOffset;
      large_vector<float> pitchLookupUsed = active->pitchLookupSmoothed;
      int smoothDelay = active->pitchBigSmoothingFilter->delay();

      if ((myEndChunk - myStartChunk) * zoomFactorX > width() - 2 * noteLabelOffset) {
        // The vibrato-polyline doesn't fit in the window
        if ((myCurrentChunk - myStartChunk) * zoomFactorX < (width() - 2 * noteLabelOffset)/2) {
          // We're at the left side of the vibrato-polyline
          windowOffset = 0 - noteLabelOffset;
        } else if ((myEndChunk - myCurrentChunk) * zoomFactorX < (width() - 2 * noteLabelOffset)/2) {
          // We're at the right side of the vibrato-polyline
          windowOffset = (myEndChunk - myStartChunk) * zoomFactorX - width() + noteLabelOffset + 1;
        } else {  // We're somewhere in the middle of the vibrato-polyline
          windowOffset = (myCurrentChunk - myStartChunk) * zoomFactorX - width()/2;
        }
      } else {  // The vibrato-polyline does fit in the window
        windowOffset = 0 - noteLabelOffset;
      }

      GLfloat *vertices;
      GLubyte *colors;
      uint verticesCounter, colorsCounter;

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      // Calculate the alternating vertical bars that indicate the vibrato periods
      if ((active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
        int color1Bars = 0;  // No. of bars with the left side at a maximum
        int color2Bars = 0;  // No. of bars with the left side at a minimum
        bool maximumFirst = true;   // The first extremum is a maximum?

        if (maximaSize + minimaSize >= 2) {  // There is at least one bar to calculate
          if (maximaSize == minimaSize) {
            if (note->maxima->at(0) < note->minima->at(0)) {
              color1Bars = maximaSize;
              color2Bars = minimaSize - 1;
              maximumFirst = true;
            } else {
              color1Bars = maximaSize - 1;
              color2Bars = minimaSize;
              maximumFirst = false;
            }
          } else {
            color1Bars = maximaSize > minimaSize ? minimaSize : maximaSize;
            color2Bars = maximaSize > minimaSize ? minimaSize : maximaSize;
            maximumFirst = maximaSize > minimaSize;
          }
        }

        float x1, x2;

        // Calculate the bars with the left side at a maximum
        vertices = new GLfloat[(color1Bars + color2Bars) * 8];
        colors = new GLubyte[(color1Bars + color2Bars) * 12];

        const char color1Red = gdata->shading1Color().red();
        const char color1Green = gdata->shading1Color().green();
        const char color1Blue = gdata->shading1Color().blue();

        verticesCounter = 0;
        colorsCounter = 0;

        for (int i = 0; i < color1Bars; i++) {
          x1 = ((((float)note->maxima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x1 < noteLabelOffset) { x1 = noteLabelOffset; }
          if (x1 > width() - noteLabelOffset) { break; }
          if (maximumFirst) {
            x2 = ((((float)note->minima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          } else {
            x2 = ((((float)note->minima->at(i+1) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          }
          if (x2 < noteLabelOffset) { continue; }
          if (x2 > width() - noteLabelOffset) { x2 = width() - noteLabelOffset; }

          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x2;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x2;
          vertices[verticesCounter++] = height();
          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = height();

          for (int j=1; j <= 4; j++) {
            colors[colorsCounter++] = color1Red;
            colors[colorsCounter++] = color1Green;
            colors[colorsCounter++] = color1Blue;
          }
        }

        // Calculate the bars with the left side at a minimum
        const char color2Red = gdata->shading2Color().red();
        const char color2Green = gdata->shading2Color().green();
        const char color2Blue = gdata->shading2Color().blue();

        for (int i = 0; i < color2Bars; i++) {
          x1 = ((((float)note->minima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x1 < noteLabelOffset) { x1 = noteLabelOffset; }
          if (x1 > width() - noteLabelOffset) { break; }
          if (maximumFirst) {
            x2 = ((((float)note->maxima->at(i+1) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          } else {
            x2 = ((((float)note->maxima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          }
          if (x2 < noteLabelOffset) { continue; }
          if (x2 > width() - noteLabelOffset) { x2 = width() - noteLabelOffset; }

          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x2;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x2;
          vertices[verticesCounter++] = height();
          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = height();

          for (int j=1; j <= 4; j++) {
            colors[colorsCounter++] = color2Red;
            colors[colorsCounter++] = color2Green;
            colors[colorsCounter++] = color2Blue;
          }
        }

        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
        glNewList(verticalPeriodBars, GL_COMPILE);
        glDrawArrays(GL_QUADS, 0, verticesCounter/2);
        glEndList();

        delete[] vertices;
        delete[] colors;


        // Calculate the vertical separator lines through the maxima
        vertices = new GLfloat[(maximaSize + minimaSize) * 4];
        colors = new GLubyte[(maximaSize + minimaSize) * 6];

        verticesCounter = 0;
        colorsCounter = 0;

        for (int i = 0; i < maximaSize; i++) {
          x1 = ((((float)note->maxima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x1 < noteLabelOffset) { continue; }
          if (x1 > width() - noteLabelOffset) { break; }

          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = height();

          colors[colorsCounter++] = 131;
          colors[colorsCounter++] = 144;
          colors[colorsCounter++] = 159;
          colors[colorsCounter++] = 131;
          colors[colorsCounter++] = 144;
          colors[colorsCounter++] = 159;
        }

        // Calculate the vertical separator lines through the minima
        for (int i = 0; i < minimaSize; i++) {
          x1 = ((((float)note->minima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x1 < noteLabelOffset) { continue; }
          if (x1 > width() - noteLabelOffset) { break; }

          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = 0;
          vertices[verticesCounter++] = x1;
          vertices[verticesCounter++] = height();

          colors[colorsCounter++] = 131;
          colors[colorsCounter++] = 144;
          colors[colorsCounter++] = 159;
          colors[colorsCounter++] = 131;
          colors[colorsCounter++] = 144;
          colors[colorsCounter++] = 159;
        }

        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
        glNewList(verticalSeparatorLines, GL_COMPILE);
        glDrawArrays(GL_LINES, 0, verticesCounter/2);
        glEndList();

        delete[] vertices;
        delete[] colors;
      }


      // Calculate the horizontal reference lines + note labels
      vertices = new GLfloat[100 * 4];
      colors = new GLubyte[100 * 6];

      verticesCounter = 0;
      colorsCounter = 0;

      const int nearestNote = toInt(avgPitch);
      QString noteLabel;
      const float referenceLineX1 = noteLabelOffset;
      const float referenceLineX2 = width() - noteLabelOffset;
      float referenceLineY;

      // Calculate the nearest reference line + note label
      referenceLineY = halfHeight + ((nearestNote - avgPitch) * zoomFactorYx100) + offsetY;

      vertices[verticesCounter++] = referenceLineX1;
      vertices[verticesCounter++] = referenceLineY;
      vertices[verticesCounter++] = referenceLineX2;
      vertices[verticesCounter++] = referenceLineY;

      colors[colorsCounter++] = 144;
      colors[colorsCounter++] = 156;
      colors[colorsCounter++] = 170;
      colors[colorsCounter++] = 144;
      colors[colorsCounter++] = 156;
      colors[colorsCounter++] = 170;

      if ((noteOctave(nearestNote) >= 0) && (noteOctave(nearestNote) <= 9)) {
        noteLabel.sprintf("%s%d", noteName(nearestNote), noteOctave(nearestNote));
      } else {
        noteLabel.sprintf("%s ", noteName(nearestNote));
      }
      noteLabels[noteLabelCounter].label = noteLabel;
      noteLabels[noteLabelCounter].y = referenceLineY;
      noteLabelCounter++;

      // Calculate as many reference lines + note labels above the note as can be seen
      for (int i = 1; ; i++) {
        referenceLineY = halfHeight + ((nearestNote + i - avgPitch) * zoomFactorYx100) + offsetY;
        if (referenceLineY > height()) { break; }

        vertices[verticesCounter++] = referenceLineX1;
        vertices[verticesCounter++] = referenceLineY;
        vertices[verticesCounter++] = referenceLineX2;
        vertices[verticesCounter++] = referenceLineY;

        colors[colorsCounter++] = 144;
        colors[colorsCounter++] = 156;
        colors[colorsCounter++] = 170;
        colors[colorsCounter++] = 144;
        colors[colorsCounter++] = 156;
        colors[colorsCounter++] = 170;

        if ((noteOctave(nearestNote + i) >= 0) && (noteOctave(nearestNote + i) <= 9)) {
          noteLabel.sprintf("%s%d", noteName(nearestNote + i), noteOctave(nearestNote + i));
        } else {
          noteLabel.sprintf("%s ", noteName(nearestNote + i));
        }
        noteLabels[noteLabelCounter].label = noteLabel;
        noteLabels[noteLabelCounter].y = referenceLineY;
        noteLabelCounter++;
      }

      // Calculate as many reference lines + note labels below the note as can be seen
      for (int i = -1; ; i--) {
        referenceLineY = halfHeight + ((nearestNote + i - avgPitch) * zoomFactorYx100) + offsetY;
        if (referenceLineY < 0) { break; }

        vertices[verticesCounter++] = referenceLineX1;
        vertices[verticesCounter++] = referenceLineY;
        vertices[verticesCounter++] = referenceLineX2;
        vertices[verticesCounter++] = referenceLineY;

        colors[colorsCounter++] = 144;
        colors[colorsCounter++] = 156;
        colors[colorsCounter++] = 170;
        colors[colorsCounter++] = 144;
        colors[colorsCounter++] = 156;
        colors[colorsCounter++] = 170;

        if ((noteOctave(nearestNote + i) >= 0) && (noteOctave(nearestNote + i) <= 9)) {
          noteLabel.sprintf("%s%d", noteName(nearestNote + i), noteOctave(nearestNote + i));
        } else {
          noteLabel.sprintf("%s ", noteName(nearestNote + i));
        }
        noteLabels[noteLabelCounter].label = noteLabel;
        noteLabels[noteLabelCounter].y = referenceLineY;
        noteLabelCounter++;
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(referenceLines, GL_COMPILE);
      glDrawArrays(GL_LINES, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate the light grey band indicating the vibratowidth according to the Prony-algorithm
      verticesCounter = 0;
      colorsCounter = 0;

      vertices = new GLfloat[(myEndChunk - myStartChunk) * 8];
      colors = new GLubyte[(myEndChunk - myStartChunk) * 16];

      for (int chunk = myStartChunk; chunk < myEndChunk - 1; chunk++) {
        float x1 = (chunk - myStartChunk) * zoomFactorX - windowOffset;
        if (x1 < noteLabelOffset) { x1 = noteLabelOffset; }
        if (x1 > width() - noteLabelOffset) { break; }
        float x2 = (chunk + 1 - myStartChunk) * zoomFactorX - windowOffset;
        if (x2 < noteLabelOffset) { continue; }
        if (x2 > width() - noteLabelOffset) { x2 = width() - noteLabelOffset; }

        float vibratoPitch1 = active->dataAtChunk(chunk)->vibratoPitch;
        float vibratoWidth1 = active->dataAtChunk(chunk)->vibratoWidth;
        float vibratoPitch2 = active->dataAtChunk(chunk + 1)->vibratoPitch;
        float vibratoWidth2 = active->dataAtChunk(chunk + 1)->vibratoWidth;
        float y1 = halfHeight + ((vibratoPitch1 + vibratoWidth1 - avgPitch) * zoomFactorYx100) + offsetY;
        float y2 = halfHeight + ((vibratoPitch1 - vibratoWidth1 - avgPitch) * zoomFactorYx100) + offsetY;
        float y3 = halfHeight + ((vibratoPitch2 - vibratoWidth2 - avgPitch) * zoomFactorYx100) + offsetY;
        float y4 = halfHeight + ((vibratoPitch2 + vibratoWidth2 - avgPitch) * zoomFactorYx100) + offsetY;

        vertices[verticesCounter++] = x1;
        vertices[verticesCounter++] = y1;
        vertices[verticesCounter++] = x1;
        vertices[verticesCounter++] = y2;
        vertices[verticesCounter++] = x2;
        vertices[verticesCounter++] = y3;
        vertices[verticesCounter++] = x2;
        vertices[verticesCounter++] = y4;

        for (int j=1; j <= 4; j++) {
          colors[colorsCounter++] = 0;
          colors[colorsCounter++] = 0;
          colors[colorsCounter++] = 0;
          colors[colorsCounter++] = 64;
        }
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(pronyWidthBand, GL_COMPILE);
      glDrawArrays(GL_QUADS, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate the average pitch according to the Prony-algorithm
      verticesCounter = 0;
      colorsCounter = 0;

      vertices = new GLfloat[(myEndChunk - myStartChunk) * 2];
      colors = new GLubyte[(myEndChunk - myStartChunk) * 4];

      for (int chunk = myStartChunk; chunk < myEndChunk; chunk++) {
        float x = (chunk - myStartChunk) * zoomFactorX - windowOffset;
        if (x < noteLabelOffset) { continue; }
        if (x > width() - noteLabelOffset) { break; }
        float y = halfHeight + ((active->dataAtChunk(chunk)->vibratoPitch - avgPitch) * zoomFactorYx100) + offsetY;

        vertices[verticesCounter++] = x;
        vertices[verticesCounter++] = y;

        colors[colorsCounter++] = 0;
        colors[colorsCounter++] = 0;
        colors[colorsCounter++] = 0;
        colors[colorsCounter++] = 127;
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(pronyAveragePitch, GL_COMPILE);
      glDrawArrays(GL_LINE_STRIP, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate the vibrato-polyline
      verticesCounter = 0;
      colorsCounter = 0;

      if ((active->doingDetailedPitch()) && (pitchLookupUsed.size() > 0)) {
        // Detailed pitch information available, calculate polyline using this info
        vertices = new GLfloat[(width() + 1) * 2];
        colors = new GLubyte[(width() + 1) * 3];

        const int pitchLookupUsedSizeLimit = pitchLookupUsed.size() - 1;
        const int beginningOfNote = myStartChunk * framesPerChunk;
        const int endOfNote = myEndChunk * framesPerChunk - 1;
        float chunk;
        float y;
        int offset;
        for (int x = noteLabelOffset; x < width() - noteLabelOffset; x++) {
          chunk = ((x + windowOffset) / zoomFactorX + myStartChunk);
          if ((chunk >= myStartChunk) && (chunk <= myEndChunk)) {
            offset = toInt(chunk * framesPerChunk + smoothDelay);
            if(offset > endOfNote) offset = endOfNote;
            if (offset < beginningOfNote + 3*smoothDelay) {
              int timeAtZero = beginningOfNote + smoothDelay;
              float scaleX = (offset - timeAtZero) / float(2*smoothDelay);
              float pitchAtZero = active->dataAtChunk(myStartChunk)->pitch;
              //float pitchAt2SmoothDelay = pitchLookupUsed.at(beginningOfNote + 3*smoothDelay);
              int smoothDelayPos3 = std::min(beginningOfNote + 3*smoothDelay, (int)pitchLookupUsed.size()-1);
              if(smoothDelayPos3 >= 0) {
                float pitchAt2SmoothDelay = pitchLookupUsed.at(smoothDelayPos3);
                y = halfHeight + (pitchAtZero + scaleX * (pitchAt2SmoothDelay - pitchAtZero) - avgPitch) * zoomFactorYx100;
              } else { y = 0; }
            //} else if (offset > endOfNote) {
            //  y = halfHeight + (pitchLookupUsed.at(endOfNote) - avgPitch) * zoomFactorYx100;
            } else {
              offset = std::min(offset, pitchLookupUsedSizeLimit);
              y = halfHeight + (pitchLookupUsed.at(offset) - avgPitch) * zoomFactorYx100;
            }

            y += offsetY;  // Vertical scrollbar offset

            vertices[verticesCounter++] = x;
            vertices[verticesCounter++] = y;

            colors[colorsCounter++] = 127;
            colors[colorsCounter++] = 0;
            colors[colorsCounter++] = 0;
          }
        }
      } else {  // No detailed pitch information available, calculate polyline using the chunkdata
        vertices = new GLfloat[(myEndChunk - myStartChunk) * 2];
        colors = new GLubyte[(myEndChunk - myStartChunk) * 3];

        float x, y;
        for (int chunk = myStartChunk; chunk < myEndChunk; chunk++) {
          x = (chunk - myStartChunk) * zoomFactorX - windowOffset;
          if (x < noteLabelOffset) { continue; }
          if (x > width() - noteLabelOffset) { break; }
          y = halfHeight + ((active->dataAtChunk(chunk)->pitch - avgPitch) * zoomFactorYx100) + offsetY;

          vertices[verticesCounter++] = x;
          vertices[verticesCounter++] = y;

          colors[colorsCounter++] = 127;
          colors[colorsCounter++] = 0;
          colors[colorsCounter++] = 0;
        }
      }
      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(vibratoPolyline, GL_COMPILE);
      glDrawArrays(GL_LINE_STRIP, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate a light grey band indicating which time is being used in the current window
      vertices = new GLfloat[8];
      colors = new GLubyte[16];

      verticesCounter = 0;
      colorsCounter = 0;

      const double halfWindowTime = (double)active->size() / (double)(active->rate() * 2);
      int pixelLeft = toInt((active->chunkAtTime(gdata->view->currentTime() - halfWindowTime) - myStartChunk) * zoomFactorX - windowOffset);
      int pixelRight = toInt((active->chunkAtTime(gdata->view->currentTime() + halfWindowTime) - myStartChunk) * zoomFactorX - windowOffset);

      vertices[verticesCounter++] = pixelLeft;
      vertices[verticesCounter++] = 0;
      vertices[verticesCounter++] = pixelRight;
      vertices[verticesCounter++] = 0;
      vertices[verticesCounter++] = pixelRight;
      vertices[verticesCounter++] = height();
      vertices[verticesCounter++] = pixelLeft;
      vertices[verticesCounter++] = height();

      for (int j=1; j <= 4; j++) {
        colors[colorsCounter++] = colorGroup().foreground().red();
        colors[colorsCounter++] = colorGroup().foreground().green();
        colors[colorsCounter++] = colorGroup().foreground().blue();
        colors[colorsCounter++] = 64;
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(currentWindowBand, GL_COMPILE);
      glDrawArrays(GL_QUADS, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate the current timeline
      vertices = new GLfloat[4];
      colors = new GLubyte[6];

      verticesCounter = 0;

      const float timeLineX = toInt((myCurrentChunk - myStartChunk) * zoomFactorX - windowOffset);

      vertices[verticesCounter++] = timeLineX;
      vertices[verticesCounter++] = 0;
      vertices[verticesCounter++] = timeLineX;
      vertices[verticesCounter++] = height();

      for (colorsCounter = 0; colorsCounter < 6; colorsCounter++) {
        colors[colorsCounter] = 0;
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(currentTimeLine, GL_COMPILE);
      glDrawArrays(GL_LINES, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;


      // Calculate the points of maxima and minima
      vertices = new GLfloat[(maximaSize + minimaSize) * 2];
      colors = new GLubyte[(maximaSize + minimaSize) * 3];

      verticesCounter = 0;
      colorsCounter = 0;

      // Calculate the maxima
      if ((active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
        float x, y;
        for (int i = 0; i < maximaSize; i++) {
          x = ((((float)note->maxima->at(i) - smoothDelay)/ framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x < noteLabelOffset) { continue; }
          if (x > width() - noteLabelOffset) { break; }
          y = halfHeight + ((pitchLookupUsed.at(note->maxima->at(i)) - avgPitch) * zoomFactorYx100) + offsetY;

          vertices[verticesCounter++] = x;
          vertices[verticesCounter++] = y;

          colors[colorsCounter++] = 255;
          colors[colorsCounter++] = 255;
          colors[colorsCounter++] = 0;
        }
      }
      // Calculate the minima
      if ((active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
        float x, y;
        for (int i = 0; i < minimaSize; i++) {
          x = ((((float)note->minima->at(i) - smoothDelay) / framesPerChunk) - myStartChunk) * zoomFactorX - windowOffset;
          if (x < noteLabelOffset) { continue; }
          if (x > width() - noteLabelOffset) { break; }
          y = halfHeight + ((pitchLookupUsed.at(note->minima->at(i)) - avgPitch) * zoomFactorYx100) + offsetY;

          vertices[verticesCounter++] = x;
          vertices[verticesCounter++] = y;

          colors[colorsCounter++] = 0;
          colors[colorsCounter++] = 255;
          colors[colorsCounter++] = 0;
        }
      }

      glVertexPointer(2, GL_FLOAT, 0, vertices);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
      glNewList(maximaMinimaPoints, GL_COMPILE);
      glDrawArrays(GL_POINTS, 0, verticesCounter/2);
      glEndList();

      delete[] vertices;
      delete[] colors;
    }
  }
}

void VibratoWidget::setZoomFactorX(double x)
{
  zoomFactorX = 2*pow10(log2(x/25));
  update();
}

void VibratoWidget::setZoomFactorY(double y)
{
  zoomFactorY = y;
  update();
}

void VibratoWidget::setOffsetY(int value)
{
  offsetY = value;
  update();
}
