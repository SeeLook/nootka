/***************************************************************************
                          vibratoperiodwidget.cpp  -  description
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
#include "vibratoperiodwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"

VibratoPeriodWidget::VibratoPeriodWidget(QWidget *parent)
  : QGLWidget(parent)
{
  prevLeftMinimumTime = -1;
  lastPeriodToDraw = -1;

  smoothedPeriods = true;
  drawSineReference = false;
  sineStyle = false;
  drawPrevPeriods = false;
  periodScaling = false;
  drawComparison = false;
}

VibratoPeriodWidget::~VibratoPeriodWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(sineReference, 1);
  for (int i = 0; i < 5; i++) {
    glDeleteLists(previousPoly[i], 1);
  }
  glDeleteLists(currentPeriod, 1);
  glDeleteLists(comparisonPoly, 1);
  glDeleteLists(comparisonReference, 1);
}

void VibratoPeriodWidget::initializeGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  sineReference = glGenLists(1);
  for (int i = 0; i < 5; i++) {
    previousPoly[i] = glGenLists(1);
  }
  currentPeriod = glGenLists(1);
  comparisonPoly = glGenLists(1);
  comparisonReference = glGenLists(1);
}

void VibratoPeriodWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  //glMatrixMode(GL_MODELVIEW);

  // Calculate the horizontal reference line
  const float halfHeight = 0.5 * height();

  glNewList(comparisonReference, GL_COMPILE);
  glColor4ub(0, 0, 0, 64);
  glBegin(GL_LINES);
  glVertex2f(0, halfHeight);
  glVertex2f(w, halfHeight);
  glEnd();
  glEndList();

  // Do forced update on resize
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::paintGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the horizontal reference line
  glLineWidth(1.5);
  glCallList(comparisonReference);

  // Draw the sinewave
  glLineWidth(2.0);
  glCallList(sineReference);

  // Draw the comparison
  glCallList(comparisonPoly);

  // Draw the previous periods
  for (int i = 4; i >= 0; i--) {
    if (lastPeriodToDraw >= i) {
      glCallList(previousPoly[i]);
    }
  }

  // Draw the current period
  glCallList(currentPeriod);

}

void VibratoPeriodWidget::doUpdate()
{
  Channel *active = gdata->getActiveChannel();

  int leftMinimumTime = -1;
  int maximumTime = -1;
  int rightMinimumTime = -1;
  int leftMinimumAt = -1;

  if ((active) && (active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);

      int smoothDelay = active->pitchBigSmoothingFilter->delay();
      int currentTime = active->chunkAtCurrentTime() * active->framesPerChunk() + smoothDelay;
      int maximaSize = note->maxima->size();
      int minimaSize = note->minima->size();

      // Determine which period to show: the 2 rightmost minima + the maximum in between
      if ((maximaSize >= 1 ) && (minimaSize == 2)) {
        // Only 2 minima
        if (currentTime >= note->minima->at(1)) {
          leftMinimumTime  = note->minima->at(0);
          rightMinimumTime = note->minima->at(1);
          leftMinimumAt = 0;
        }
      } else if ((maximaSize >= 1) && (minimaSize > 2)) {
        // More than 2 minima
        for (int i = 2; i < minimaSize; i++) {
          if ((currentTime >= note->minima->at(i-1)) && (currentTime <= note->minima->at(i))) {
            leftMinimumTime  = note->minima->at(i-2);
            rightMinimumTime = note->minima->at(i-1);
            leftMinimumAt = i-2;
            break;
          }
        }
        if (currentTime > note->minima->at(minimaSize - 1)) {
          leftMinimumTime  = note->minima->at(minimaSize - 2);
          rightMinimumTime = note->minima->at(minimaSize - 1);
          leftMinimumAt = minimaSize - 2;
        }
      }
      // The maximum in between
      for (int i = 0; i < maximaSize; i++) {
        if ((note->maxima->at(i) >= leftMinimumTime) && (note->maxima->at(i) <= rightMinimumTime)) {
          maximumTime = note->maxima->at(i);
          break;
        }
      }
    }
  }

  if (prevLeftMinimumTime == leftMinimumTime) {
    // Same period as previous, don't change the polys & no update needed

  } else {
    // Period has changed

    makeCurrent();

    if (leftMinimumTime == -1) {
      // No period to draw, erase polys & update

      glNewList(sineReference, GL_COMPILE);
      glEndList();

      for (int i = 0; i < 5; i++) {
        glNewList(previousPoly[i], GL_COMPILE);
        glEndList();
      }

      glNewList(currentPeriod, GL_COMPILE);
      glEndList();

      glNewList(comparisonPoly, GL_COMPILE);
      glEndList();

      prevLeftMinimumTime = -1;

      updateGL();

    } else {
      // New period, calculate new polys & update

      glNewList(sineReference, GL_COMPILE);
      glEndList();

      for (int i = 0; i < 5; i++) {
        glNewList(previousPoly[i], GL_COMPILE);
        glEndList();
      }

      glNewList(currentPeriod, GL_COMPILE);
      glEndList();

      glNewList(comparisonPoly, GL_COMPILE);
      glEndList();

      AnalysisData *data = active->dataAtCurrentChunk();
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);

      large_vector<float> thePitchLookup;
      int theDelay;
      if(smoothedPeriods) {
        thePitchLookup = active->pitchLookupSmoothed;
        theDelay = 0;
      } else {
        thePitchLookup = active->pitchLookup;
        theDelay = active->pitchBigSmoothingFilter->delay() - active->pitchSmallSmoothingFilter->delay();
      }

      int theLeftMinimumTime = leftMinimumTime - theDelay;
      int theRightMinimumTime = rightMinimumTime - theDelay;
      int theMaximumTime = maximumTime - theDelay;
      int periodDuration = theRightMinimumTime - theLeftMinimumTime;

      float minimumPitch = (thePitchLookup.at(theLeftMinimumTime) >
                            thePitchLookup.at(theRightMinimumTime))
                          ? thePitchLookup.at(theRightMinimumTime)
                          : thePitchLookup.at(theLeftMinimumTime);
      float maximumPitch = thePitchLookup.at(theMaximumTime);
      float periodWidth = maximumPitch - minimumPitch;

      const float halfHeight = 0.5 * height();

      // Display either sine- or cosine-style periods
      int theSineDelay = 0;
      if (sineStyle) {
        theSineDelay = toInt((rightMinimumTime - leftMinimumTime) * 0.25);
      }

      GLfloat *vertices;
      vertices = new GLfloat[(width() + 1) * 2];

      GLfloat *sineVertices;
      sineVertices = new GLfloat[(width() + 1) * 2];

      GLubyte *colors;
      colors = new GLubyte[(width() + 1) * 4];
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);


      // Calculate the sinewave
      if (drawSineReference && (width() > 0) && (height() > 0)) {
        uint v = 0;
        uint c = 0;
        if (sineStyle) {
          for (float xx = 0; xx < width(); xx++) {
            sineVertices[v++] = 0.05 * width() + 0.9 * xx;
            sineVertices[v++] = halfHeight + halfHeight * 0.9 * sin((xx/width()) * 2 * PI);
            colors[c++] = 255;
            colors[c++] = 255;
            colors[c++] = 0;
            colors[c++] = 255;
          }
        } else {
          for (float xx = 0; xx < width(); xx++) {
            sineVertices[v++] = 0.05 * width() + 0.9 * xx;;
            sineVertices[v++] = halfHeight + halfHeight * -0.9 * cos((xx/width()) * 2 * PI);
            colors[c++] = 255;
            colors[c++] = 255;
            colors[c++] = 0;
            colors[c++] = 255;
          }
        }
        glVertexPointer(2, GL_FLOAT, 0, sineVertices);
        glNewList(sineReference, GL_COMPILE);
        glDrawArrays(GL_LINE_STRIP, 0, width());
        glEndList();
      }


      // Calculate the previous period(s)
      if (drawPrevPeriods && (width() > 0) && (height() > 0)) {
        for (int i = 0; i < 5; i++) {
          if (leftMinimumAt - (i+1) < 0) { 
            lastPeriodToDraw = i - 1;
            break;
          }

          uint v = 0;
          uint c = 0;
          int thisPrevLeftMinimumTime = note->minima->at(leftMinimumAt - (i+1)) - theDelay;
          int thisPrevRightMinimumTime = note->minima->at(leftMinimumAt - i) - theDelay;
          int thisPrevDuration = thisPrevRightMinimumTime - thisPrevLeftMinimumTime;
          int thisPrevMaximumTime = 0;
          for (int j = 0; j < note->maxima->size(); j++) {
            if ((note->maxima->at(j) >= thisPrevLeftMinimumTime) && (note->maxima->at(j) <= thisPrevRightMinimumTime)) {
              thisPrevMaximumTime = note->maxima->at(j) - theDelay;
              break;
            }
          }

          float thisPrevMinimumPitch = (thePitchLookup.at(thisPrevLeftMinimumTime) >
                                        thePitchLookup.at(thisPrevRightMinimumTime))
                                      ? thePitchLookup.at(thisPrevRightMinimumTime)
                                      : thePitchLookup.at(thisPrevLeftMinimumTime);
          float thisPrevMaximumPitch = thePitchLookup.at(thisPrevMaximumTime);
          float thisPrevWidth = thisPrevMaximumPitch - thisPrevMinimumPitch;

          if (periodScaling) {
            for (float xx = 0; xx < width(); xx++) {
              int offset = toInt((xx / width()) * thisPrevDuration + thisPrevLeftMinimumTime + theSineDelay);
              vertices[v++] = 0.05 * width() + 0.9 * xx;
              vertices[v++] = 0.05 * height() + 0.9 * ((thePitchLookup.at(offset) - thisPrevMinimumPitch) / thisPrevWidth) * height();
              colors[c++] = 127;
              colors[c++] = 0;
              colors[c++] = 0;
              colors[c++] = toInt(float(1/pow(2,i+1)) * 255);
            }
            glVertexPointer(2, GL_FLOAT, 0, vertices);
            glNewList(previousPoly[i], GL_COMPILE);
            glDrawArrays(GL_LINE_STRIP, 0, width());
            glEndList();
          } else {
            for (float xx = 0; xx < width(); xx++) {
              int offset = toInt((xx / width()) * thisPrevDuration + thisPrevLeftMinimumTime + theSineDelay);
              float xxx = xx * (float(thisPrevDuration) / periodDuration);
              xxx = xxx + ((0.5 * (periodDuration - thisPrevDuration) / periodDuration) * width());
              vertices[v++] = 0.05 * width() + 0.9 * xxx;
              vertices[v++] = 0.05 * height() + 0.9 * ((thePitchLookup.at(offset) - minimumPitch) / periodWidth) * height();
              colors[c++] = 127;
              colors[c++] = 0;
              colors[c++] = 0;
              colors[c++] = toInt(float(1/pow(2,i+1)) * 255);
            }

            glVertexPointer(2, GL_FLOAT, 0, vertices);
            glNewList(previousPoly[i], GL_COMPILE);
            glDrawArrays(GL_LINE_STRIP, 0, width());
            glEndList();
          }
          lastPeriodToDraw = i;
        }
      }


      // Calculate the current period
      if ((width() > 0) && (height() > 0)) {
        uint v = 0;
        uint c = 0;
        for (float xx = 0; xx < width(); xx++) {
          vertices[v++] = 0.05 * width() + 0.9 * xx;
          vertices[v++] = 0.05 * height() + 0.9 * ((thePitchLookup.at(toInt((xx / width()) * periodDuration + theLeftMinimumTime + theSineDelay)) - minimumPitch) / periodWidth) * height();
          colors[c++] = 127;
          colors[c++] = 0;
          colors[c++] = 0;
          colors[c++] = 255;
        }
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glNewList(currentPeriod, GL_COMPILE);
        glDrawArrays(GL_LINE_STRIP, 0, width());
        glEndList();
      }


      // Calculate the comparison
      if (drawComparison && drawSineReference && (width() > 0) && (height() > 0)) {
        uint v = 0;
        uint c = 0;
        for (int xx = 0; xx < width(); xx++) {
          vertices[v++] = 0.05 * width() + 0.9 * xx;
          vertices[v] = halfHeight + vertices[v] - sineVertices[v]; ++v;
          colors[c++] = 0;
          colors[c++] = 255;
          colors[c++] = 0;
          colors[c++] = 255;
        }
        glVertexPointer(2, GL_FLOAT, 0, vertices);
        glNewList(comparisonPoly, GL_COMPILE);
        glDrawArrays(GL_LINE_STRIP, 0, width());
        glEndList();
      }

      delete[] vertices;
      delete[] sineVertices;
      delete[] colors;

      prevLeftMinimumTime = leftMinimumTime;

      updateGL();
    }
  }
}

void VibratoPeriodWidget::setSmoothedPeriods(bool value)
{
  smoothedPeriods = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::setDrawSineReference(bool value)
{
  drawSineReference = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::setSineStyle(bool value)
{
  sineStyle = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::setDrawPrevPeriods(bool value)
{
  drawPrevPeriods = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::setPeriodScaling(bool value)
{
  periodScaling = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}

void VibratoPeriodWidget::setDrawComparison(bool value)
{
  drawComparison = value;
  prevLeftMinimumTime = -1;
  doUpdate();
}
