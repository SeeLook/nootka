/***************************************************************************
                          vibratocirclewidget.cpp  -  description
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
#include "vibratocirclewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"

VibratoCircleWidget::VibratoCircleWidget(QWidget *parent)
  : QGLWidget(parent)
{
  accuracy = 1.0;
  type = 5;
  lastPeriodToDraw = -1;
}

VibratoCircleWidget::~VibratoCircleWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(referenceCircle, 1);
  glDeleteLists(currentPeriod, 1);
  for (int i = 0; i < 6; i++) {
    glDeleteLists(prevPeriods[i], 1);
  }
}

void VibratoCircleWidget::initializeGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  referenceCircle = glGenLists(1);
  currentPeriod = glGenLists(1);
  for (int i = 0; i < 6; i++) {
    prevPeriods[i] = glGenLists(1);
  }
}

void VibratoCircleWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  //glMatrixMode(GL_MODELVIEW);


  // Calculate reference circle + lines
  glNewList(referenceCircle, GL_COMPILE);

  const float halfWidth = 0.5 * width();
  const float halfHeight = 0.5 * height();

  // The horizontal line
  glColor4ub(0, 0, 0, 64);
  glBegin(GL_LINES);
  glVertex2f(0, halfHeight);
  glVertex2f(w, halfHeight);
  glEnd();

  // The vertical line
  glColor4ub(0, 0, 0, 64);
  glBegin(GL_LINES);
  glVertex2f(halfWidth, 0);
  glVertex2f(halfWidth, h);
  glEnd();

  // The circle
  const float DEG2RAD = PI / 180;
  glColor3ub(0, 0, 0);
  glBegin(GL_LINE_LOOP);
  for (int i=0; i < 360; i=i+1)
  {
    float degInRad = (float)i * DEG2RAD;
    glVertex2f(halfWidth + 0.8 * halfWidth * cos(degInRad), halfHeight + 0.8 * halfHeight * sin(degInRad));
  }
  glEnd();

  glEndList();


  // Do forced update on resize
  doUpdate();
}

void VibratoCircleWidget::paintGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the reference circle
  glLineWidth(1.5);
  //glColor3ub(0, 0, 255);
  glCallList(referenceCircle);

  if ((type == 1) || (type == 2) || (type == 4) || (type == 5)) {
    // Draw the current period
    glLineWidth(2.0);
    glCallList(currentPeriod);
  } else if (type == 3) {
    // Draw the previous periods
    for (int i = 5; i >= 0; i--) {
      if (lastPeriodToDraw >= i) {
        glCallList(prevPeriods[i]);
      }
    }
  }
}

void VibratoCircleWidget::doUpdate()
{
  makeCurrent();

  glNewList(currentPeriod, GL_COMPILE);
  glEndList();

  for (int i = 0; i < 6; i++) {
    glNewList(prevPeriods[i], GL_COMPILE);
    glEndList();
  }

  Channel *active = gdata->getActiveChannel();

  int leftMinimumTime = -1;
  int maximumTime = -1;
  int rightMinimumTime = -1;
  int leftMinimumAt = -1;
  int maximumAt = -1;

  if ((active) && (active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);

      // Determine which delay to use
      int smoothDelay = active->pitchBigSmoothingFilter->delay();
      large_vector<float> pitchLookupUsed = active->pitchLookupSmoothed;

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
          maximumAt = i;
          break;
        }
      }

      if (maximumTime > 0) {
        // There is at least one full period before this one
        const float halfWidth = 0.5 * width();
        const float halfHeight = 0.5 * height();

        uint verticesCounter, colorsCounter;

        if ((type == 1) || (type == 2)) {
          const int stepSize = active->rate() / 1000;  // Draw element for every 0.001s
          const int prevPeriodDuration = rightMinimumTime - leftMinimumTime;
          const int currentChunk = active->chunkAtCurrentTime();

          float prevMinimumPitch = (pitchLookupUsed.at(leftMinimumTime) >
                                    pitchLookupUsed.at(rightMinimumTime))
                                  ? pitchLookupUsed.at(rightMinimumTime)
                                  : pitchLookupUsed.at(leftMinimumTime);  // Minimum pitch of previous period
          float prevMaximumPitch = pitchLookupUsed.at(maximumTime);       // Maximum pitch of previous period
          float prevAvgPitch = (prevMinimumPitch + prevMaximumPitch) * 0.5;
          float prevWidth = prevMaximumPitch - prevMinimumPitch;

          int arraySize = 1 + (((currentChunk+1) * active->framesPerChunk() + smoothDelay - 1) - rightMinimumTime) / stepSize;

          GLfloat *vertices;
          vertices = new GLfloat[arraySize * 2];

          GLubyte *colors;
          colors = new GLubyte[arraySize * 4];

          verticesCounter = 0;
          colorsCounter = 0;

          int end_i = std::min((currentChunk+1) * active->framesPerChunk() + smoothDelay, (int)pitchLookupUsed.size());
          for (int i = rightMinimumTime; i < end_i; i += stepSize) {

            if (i > (int)pitchLookupUsed.size()) {
              // Break out of loop when end of smoothed pitchlookup is reached
              break;
            }

            if ((i - rightMinimumTime) > prevPeriodDuration) {
              // Break out of loop when current period is longer than previous period, 'circle' is complete then
              break;
            }

            float currentPitch = pitchLookupUsed.at(i);
            float prevPitch = pitchLookupUsed.at(i - prevPeriodDuration);

            switch (type) {
              case 1: {
                float phase = float(i - rightMinimumTime) / prevPeriodDuration;
                float cosPhase = cos(phase * twoPI);
                float sinPhase = sin(phase * twoPI);
                float comparison = (currentPitch - prevPitch) / prevWidth;

                vertices[verticesCounter++] = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
                vertices[verticesCounter++] = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);

                colors[colorsCounter++] = 255;
                colors[colorsCounter++] = 0;
                colors[colorsCounter++] = 0;
                colors[colorsCounter++] = 255;
                break;
              }
              case 2: {
                float phase = float(i - rightMinimumTime) / prevPeriodDuration;
                float cosPhase = cos(phase * twoPI);
                float sinPhase = sin(phase * twoPI);
                float comparison = (currentPitch - (prevAvgPitch + (-cosPhase * prevWidth * 0.5))) / prevWidth;

                vertices[verticesCounter++] = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
                vertices[verticesCounter++] = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);

                colors[colorsCounter++] = 255;
                colors[colorsCounter++] = 0;
                colors[colorsCounter++] = 0;
                colors[colorsCounter++] = 255;
                break;
              }
              default: {
                break;
              }
            }
          }

          glVertexPointer(2, GL_FLOAT, 0, vertices);
          glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
          glNewList(currentPeriod, GL_COMPILE);
          glDrawArrays(GL_LINE_STRIP, 0, verticesCounter / 2);
          glEndList();

          delete[] vertices;
          delete[] colors;
        } else if (type == 3) {
          const int stepSize = active->rate() / 1000;  // Draw element for every 0.001s
          for (int p = 0; p < 6; p++) {
            if (leftMinimumAt - p < 0) { 
              lastPeriodToDraw = p - 1;
              break;
            }

            leftMinimumTime = note->minima->at(leftMinimumAt - p);
            rightMinimumTime = note->minima->at(leftMinimumAt - (p - 1));
            for (int j = 0; j < note->maxima->size(); j++) {
              if ((note->maxima->at(j) >= leftMinimumTime) && (note->maxima->at(j) <= rightMinimumTime)) {
                maximumTime = note->maxima->at(j);
                break;
              }
            }

            float prevMinimumPitch = (pitchLookupUsed.at(leftMinimumTime) >
                                pitchLookupUsed.at(rightMinimumTime))
                              ? pitchLookupUsed.at(rightMinimumTime)
                              : pitchLookupUsed.at(leftMinimumTime);
            float prevMaximumPitch = pitchLookupUsed.at(maximumTime);
            float prevAvgPitch = (prevMinimumPitch + prevMaximumPitch) * 0.5;
            float prevWidth = prevMaximumPitch - prevMinimumPitch;

            int arraySize = 1 + (rightMinimumTime - leftMinimumTime) / stepSize;

            GLfloat *vertices;
            vertices = new GLfloat[arraySize * 2];

            GLubyte *colors;
            colors = new GLubyte[arraySize * 4];

            verticesCounter = 0;
            colorsCounter = 0;

            for (int i = leftMinimumTime; i <= rightMinimumTime; i += stepSize) {

              float pitch = pitchLookupUsed.at(i);

              float phase = float(i - leftMinimumTime) / float(rightMinimumTime - leftMinimumTime);
              float cosPhase = cos(phase * twoPI);
              float sinPhase = sin(phase * twoPI);
              float comparison = (pitch - (prevAvgPitch + (-cosPhase * prevWidth * 0.5))) / prevWidth;

              vertices[verticesCounter++] = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
              vertices[verticesCounter++] = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);

              colors[colorsCounter++] = 255;
              colors[colorsCounter++] = 0;
              colors[colorsCounter++] = 0;
              colors[colorsCounter++] = toInt(float(1/pow(2,p)) * 255);
            }

            glVertexPointer(2, GL_FLOAT, 0, vertices);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glNewList(prevPeriods[p], GL_COMPILE);
            glDrawArrays(GL_LINE_STRIP, 0, verticesCounter / 2);
            glEndList();

            delete[] vertices;
            delete[] colors;

            lastPeriodToDraw = p;
          }
        } else if (type == 4) {

          const int periodDuration = rightMinimumTime - leftMinimumTime;
          int arraySize = 362;

          GLfloat *vertices;
          vertices = new GLfloat[arraySize * 2];

          GLubyte *colors;
          colors = new GLubyte[arraySize * 4];

          verticesCounter = 0;

          vertices[verticesCounter++] = halfWidth;
          vertices[verticesCounter++] = halfHeight;

          for (int j = 0; j < 360; j++) {
            int i = leftMinimumTime + toInt(floor((j / float(360)) * periodDuration));
            if (i > rightMinimumTime) { i = rightMinimumTime; }

            float pitch = pitchLookupUsed.at(i);

            float phase = float(i - leftMinimumTime) / float(periodDuration);
            float cosPhase = cos(phase * twoPI);
            float sinPhase = sin(phase * twoPI);
            float minStep = float(i - leftMinimumTime) * ((pitchLookupUsed.at(rightMinimumTime) - pitchLookupUsed.at(leftMinimumTime)) / periodDuration);
            float thisMinimumPitch = pitchLookupUsed.at(leftMinimumTime) + minStep;
            float thisMaximumPitch = pitchLookupUsed.at(maximumTime);
            float thisAvgPitch = (thisMinimumPitch + thisMaximumPitch) * 0.5;
            float thisWidth = thisMaximumPitch - thisMinimumPitch;
            float comparison = (pitch - (thisAvgPitch + (-cosPhase * thisWidth * 0.5))) / thisWidth;

            vertices[verticesCounter++] = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
            vertices[verticesCounter++] = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);
          }

          vertices[verticesCounter++] = 1.8 * halfWidth;
          vertices[verticesCounter++] = halfHeight;

          for (int i = 0; i < 4*362; i += 4) {
            colors[i] = 255;
            colors[i+1] = 0;
            colors[i+2] = 0;
            colors[i+3] = 200;
          }

          glVertexPointer(2, GL_FLOAT, 0, vertices);
          glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
          glNewList(currentPeriod, GL_COMPILE);
          glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
          glEndList();

          delete[] vertices;
          delete[] colors;

        } else if (type == 5) {

          if (leftMinimumAt == 0) {  // Fade in first period
            const int periodDuration = rightMinimumTime - leftMinimumTime;
            const int fadeAlphaInt = 200 * (currentTime - rightMinimumTime) / periodDuration;
            const GLubyte fadeAlpha = (fadeAlphaInt > 255) ? 255 : fadeAlphaInt;

            int arraySize = 362;

            GLfloat *vertices;
            vertices = new GLfloat[arraySize * 2];

            GLubyte *colors;
            colors = new GLubyte[arraySize * 4];

            verticesCounter = 0;

            vertices[verticesCounter++] = halfWidth;
            vertices[verticesCounter++] = halfHeight;

            for (int j = 0; j < 360; j++) {
              int i = leftMinimumTime + toInt(floor((j / float(360)) * periodDuration));
              if (i > rightMinimumTime) { i = rightMinimumTime; }

              float pitch = pitchLookupUsed.at(i);

              float phase = float(i - leftMinimumTime) / float(periodDuration);
              float cosPhase = cos(phase * twoPI);
              float sinPhase = sin(phase * twoPI);
              float minStep = float(i - leftMinimumTime) * ((pitchLookupUsed.at(rightMinimumTime) - pitchLookupUsed.at(leftMinimumTime)) / periodDuration);
              float thisMinimumPitch = pitchLookupUsed.at(leftMinimumTime) + minStep;
              float thisMaximumPitch = pitchLookupUsed.at(maximumTime);
              float thisAvgPitch = (thisMinimumPitch + thisMaximumPitch) * 0.5;
              float thisWidth = thisMaximumPitch - thisMinimumPitch;
              float comparison = (pitch - (thisAvgPitch + (-cosPhase * thisWidth * 0.5))) / thisWidth;

              vertices[verticesCounter++] = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
              vertices[verticesCounter++] = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);
            }

            vertices[verticesCounter++] = 1.8 * halfWidth;
            vertices[verticesCounter++] = halfHeight;

            for (int i = 0; i < 4*362; i += 4) {
              colors[i] = 255;
              colors[i+1] = 0;
              colors[i+2] = 0;
              colors[i+3] = fadeAlpha;
            }

            glVertexPointer(2, GL_FLOAT, 0, vertices);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glNewList(currentPeriod, GL_COMPILE);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
            glEndList();

            delete[] vertices;
            delete[] colors;

          } else if (leftMinimumAt > 0) {  // Morph from previous period to current period

            const int prevLeftMinimumTime = note->minima->at(leftMinimumAt - 1);
            const int prevRightMinimumTime = leftMinimumTime;
            const int prevMaximumTime = note->maxima->at(maximumAt - 1);
            const int prevPeriodDuration = prevRightMinimumTime - prevLeftMinimumTime;
            float prevMaximumPitch = pitchLookupUsed.at(prevMaximumTime);
            const int periodDuration = rightMinimumTime - leftMinimumTime;
            float thisMaximumPitch = pitchLookupUsed.at(maximumTime);
            float distanceRatio = (currentTime - rightMinimumTime) / float(periodDuration);
            if (distanceRatio > 1) { distanceRatio = 1; }

            int arraySize = 362;

            GLfloat *vertices;
            vertices = new GLfloat[arraySize * 2];

            GLubyte *colors;
            colors = new GLubyte[arraySize * 4];

            verticesCounter = 0;

            vertices[verticesCounter++] = halfWidth;
            vertices[verticesCounter++] = halfHeight;

            for (int j = 0; j < 360; j++) {
              int prevI = prevLeftMinimumTime + toInt(floor((j / float(360)) * prevPeriodDuration));
              if (prevI > prevRightMinimumTime) { prevI = prevRightMinimumTime; }
              int i = leftMinimumTime + toInt(floor((j / float(360)) * periodDuration));
              if (i > rightMinimumTime) { i = rightMinimumTime; }

              float prevPitch = pitchLookupUsed.at(prevI);
              float prevPhase = float(prevI - prevLeftMinimumTime) / float(prevPeriodDuration);
              float prevCosPhase = cos(prevPhase * twoPI);
              float prevSinPhase = sin(prevPhase * twoPI);
              float prevMinStep = float(prevI - prevLeftMinimumTime) * ((pitchLookupUsed.at(prevRightMinimumTime) - pitchLookupUsed.at(prevLeftMinimumTime)) / prevPeriodDuration);
              float prevMinimumPitch = pitchLookupUsed.at(prevLeftMinimumTime) + prevMinStep;
              float prevAvgPitch = (prevMinimumPitch + prevMaximumPitch) * 0.5;
              float prevWidth = prevMaximumPitch - prevMinimumPitch;
              float prevComparison = (prevPitch - (prevAvgPitch + (-prevCosPhase * prevWidth * 0.5))) / prevWidth;
              float prevX = halfWidth + halfWidth * (0.8 * prevCosPhase + accuracy * prevComparison * prevCosPhase);
              float prevY = halfHeight + halfHeight * (0.8 * prevSinPhase + accuracy * prevComparison * prevSinPhase);

              float pitch = pitchLookupUsed.at(i);
              float phase = float(i - leftMinimumTime) / float(periodDuration);
              float cosPhase = cos(phase * twoPI);
              float sinPhase = sin(phase * twoPI);
              float minStep = float(i - leftMinimumTime) * ((pitchLookupUsed.at(rightMinimumTime) - pitchLookupUsed.at(leftMinimumTime)) / periodDuration);
              float thisMinimumPitch = pitchLookupUsed.at(leftMinimumTime) + minStep;
              float thisAvgPitch = (thisMinimumPitch + thisMaximumPitch) * 0.5;
              float thisWidth = thisMaximumPitch - thisMinimumPitch;
              float comparison = (pitch - (thisAvgPitch + (-cosPhase * thisWidth * 0.5))) / thisWidth;
              float thisX = halfWidth + halfWidth * (0.8 * cosPhase + accuracy * comparison * cosPhase);
              float thisY = halfHeight + halfHeight * (0.8 * sinPhase + accuracy * comparison * sinPhase);

              vertices[verticesCounter++] = prevX + distanceRatio * (thisX - prevX);
              vertices[verticesCounter++] = prevY + distanceRatio * (thisY - prevY);
            }

            vertices[verticesCounter++] = 1.8 * halfWidth;
            vertices[verticesCounter++] = halfHeight;

            for (int i = 0; i < 4*362; i += 4) {
              colors[i] = 255;
              colors[i+1] = 0;
              colors[i+2] = 0;
              colors[i+3] = 200;
            }

            glVertexPointer(2, GL_FLOAT, 0, vertices);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glNewList(currentPeriod, GL_COMPILE);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
            glEndList();

            delete[] vertices;
            delete[] colors;
          }
        }
      }
    }
  }
  updateGL();
}

void VibratoCircleWidget::setAccuracy(int value)
{
  accuracy = value / 10.0;
  doUpdate();
}

void VibratoCircleWidget::setType(int value)
{
  type = value;
  doUpdate();
}
