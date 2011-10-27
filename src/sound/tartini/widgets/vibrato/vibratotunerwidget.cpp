/***************************************************************************
                          vibratotunerwidget.cpp  -  description
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
#include "vibratotunerwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "musicnotes.h"
  
VibratoTunerWidget::VibratoTunerWidget(QWidget *parent)
  : QGLWidget(parent)
{
  needleValueToDraw = -999;
  prevNeedleValue = -999;
  prevClosePitch = 0;
  curPitch = 0.0;
  tunerLabelCounter = 0;
  for (int i = 0; i < 100; i++) {
    tunerLabels[i].label = QString(8,' ');
    tunerLabels[i].x = 0.0f;
    tunerLabels[i].y = 0.0f;
  }
  tunerFont = QFont();
  tunerFont.setPointSize(9);
}

VibratoTunerWidget::~VibratoTunerWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(dial, 1);
  glDeleteLists(needle, 1);
}

void VibratoTunerWidget::initializeGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  dial = glGenLists(1);
  needle = glGenLists(1);

}

void VibratoTunerWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  //glMatrixMode(GL_MODELVIEW);

  // Calculate the dial with the new width & height
  const float halfWidth = 0.5 * width();
  const float halfHeight = 0.5 * height();
  const float radius = 1.8 * MAX(halfHeight, halfWidth);
  const float theta = asin(float(width()) / (2 * radius));
  const float rho = 0.5 * PI - theta;
  const float centerX = halfWidth;
  const float centerY = height() - radius;

  glNewList(dial, GL_COMPILE);

  // Calculate the coordinates of the points on the upper arc
  float *upperArcX, *upperArcY;
  upperArcX = new float[width()];
  upperArcY = new float[width()];
  float upperArcTheta = -theta;
  float upperArcThetaStep = (2 * theta) / width();
  for (int x = 0; x < width(); x++) {
    upperArcX[x] = x;
    upperArcY[x] = centerY + (sin(0.5*PI - upperArcTheta) * radius);
    upperArcTheta += upperArcThetaStep;
  }

  // Calculate the white semicircle of the dial
  glBegin(GL_TRIANGLE_FAN);
  glColor3ub(255, 255, 255);
  glVertex2f(centerX, centerY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(upperArcX[x], upperArcY[x]);
  }
  glEnd();

  // Calculate the black outline of the dial
  glBegin(GL_LINE_LOOP);
  glColor3ub(0, 0, 0);
  glVertex2f(centerX, centerY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(upperArcX[x], upperArcY[x]);
  }
  glEnd();

  // Calculate the black arc inside the dial
  glBegin(GL_LINE_STRIP);
  glColor3ub(0, 0, 0);
  float lowerArcTheta = -theta;
  float lowerArcThetaStep = (2 * theta) / halfWidth;
  for (float x = 0.25 * width(); x < 0.75 * width(); x++) {
    glVertex2f(x, centerY + (0.5 * sin(0.5*PI - lowerArcTheta) * radius));
    lowerArcTheta += lowerArcThetaStep;
  }
  glEnd();

  // Calculate line markings & text labels
  float step = (2 * theta) / 12.0;
  tunerLabelCounter = 0;
  char *tunerLabelLookup[11] = { "+50   ", "+40", "+30", "+20", "+10", "0",
                                 "-10", "-20", "-30", "-40", "   -50" };

  for (int j=0; j<11; j++) {
    float i = rho + step * (j+1);
    float startX = centerX + radius * cos(i);
    float startY = centerY + radius * sin(i);
    if (width() < 350) {  // Small dial
      if ((j==0) || (j==3) || (j==5) || (j==7) || (j==10)) {
        // Bigger marking + text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.05 * (centerX - startX), startY + 0.05 * (centerY - startY));
        glEnd();

        tunerLabels[tunerLabelCounter].x = startX + 0.08 * (centerX - startX);
        tunerLabels[tunerLabelCounter].y = startY + 0.08 * (centerY - startY);
        tunerLabels[tunerLabelCounter++].label = tunerLabelLookup[j];
      } else {
        // Smaller marking, no text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.03 * (centerX - startX), startY + 0.03 * (centerY - startY));
        glEnd();
      }
    } else {  // Big dial
      // Bigger marking + text label
      tunerLabelLookup[0] = "+50";
      tunerLabelLookup[11] = "-50";

      glBegin(GL_LINES);
      glColor3ub(0, 0, 0);
      glVertex2f(startX, startY);
      glVertex2f(startX + 0.05 * (centerX - startX), startY + 0.05 * (centerY - startY));
      glEnd();

      tunerLabels[tunerLabelCounter].x = startX + 0.08 * (centerX - startX);
      tunerLabels[tunerLabelCounter].y = startY + 0.08 * (centerY - startY);
      tunerLabels[tunerLabelCounter++].label = tunerLabelLookup[j];
    }
  }

  // Calculate the position of the "Cents" label
  centsLabelX = centerX;
  centsLabelY = centerY + 0.7 * radius;

  glEndList();

  delete[] upperArcX;
  delete[] upperArcY;

  // Do forced update on resize
  needleValueToDraw = -999;
  prevNeedleValue = -999;
  prevClosePitch = -1;
  doUpdate(curPitch);
}

void VibratoTunerWidget::paintGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the dial
  glLineWidth(1.5);
  glCallList(dial);

  // Draw the labels
  QFontMetrics fm = QFontMetrics(tunerFont);

  glColor3ub(0,0,0);
  renderText(centsLabelX - (0.5 * fm.width("Cents")), centsLabelY, 0, "Cents", tunerFont);
  for (int i = 0; i < tunerLabelCounter; i++) {
    renderText(tunerLabels[i].x - (0.5 * fm.width(tunerLabels[i].label)), tunerLabels[i].y - 8, 0, tunerLabels[i].label, tunerFont);
  }

  // Draw the needle
  glLineWidth(1.0);
  glCallList(needle);

}

void VibratoTunerWidget::doUpdate(double thePitch)
{
  curPitch = thePitch;

  Channel *active = gdata->getActiveChannel();

  float needleValue = 0;
  int closePitch = 0;
  bool isPitchBlackNote = true;

  if (active) {
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);
/*
      large_vector<float> pitchLookupUsed = active->pitchLookupSmoothed;
      int smoothDelay = active->pitchBigSmoothingFilter->delay();

      float myPitch = 0.0;
      const uint pitchLookupUsedSizeLimit = pitchLookupUsed.size() - 1;
      const uint endOfNote = note->endChunk() * active->framesPerChunk() - 1;
      if ((active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
        // Detailed pitch information available, determine pitch using this info
        uint offset = active->currentChunk() * active->framesPerChunk() + smoothDelay;
        if (offset > endOfNote) {
          myPitch = pitchLookupUsed.at(endOfNote);
        } else {
          uint intTime = std::min(offset, pitchLookupUsedSizeLimit);
          myPitch = pitchLookupUsed.at(intTime);
        }
      } else {
        // No detailed pitch information available, determine pitch using the chunkdata
        myPitch = active->dataAtCurrentChunk()->pitch;
      }

      // We can work out how many semitones from A the note is
      closePitch = toInt(myPitch);
      isPitchBlackNote = isBlackNote(closePitch);
      needleValue = 100 * (myPitch - float(closePitch));
*/
    }
  }

  closePitch = toInt(thePitch);
  isPitchBlackNote = isBlackNote(closePitch);
  needleValue = 100 * (thePitch - float(closePitch));

  if ((fabs(prevNeedleValue - needleValue) < 0.05) && (prevClosePitch == closePitch)) {
    // Pitch hasn't changed (much), no update needed

  } else {
    // Pitch has changed

    makeCurrent();

    glNewList(needle, GL_COMPILE);
    glEndList();

    if (closePitch == 0) {
      // No pitch, don't draw the needle this update

      prevNeedleValue = -999;
      prevClosePitch = 0;

      needleValueToDraw = -999;

      resetLeds();

      updateGL();

    } else {
      // Pitch, draw the needle this update

      int VTLEDLetterLookup[12] = { 2, 2, 3, 3, 4, 5, 5, 6, 6, 0, 0, 1 };
      resetLeds();
      emit(ledSet(VTLEDLetterLookup[noteValue(closePitch)], true));
      if(isBlackNote(closePitch)) { emit(ledSet(7, true)); }

      prevNeedleValue = needleValue;
      prevClosePitch = closePitch;
      prevIsPitchBlackNote = isPitchBlackNote;

      needleValueToDraw = needleValue;

      // Calculate the needle, if needleValueToDraw is set to sensible value
      if((needleValueToDraw > -60.0) && (needleValueToDraw < 60.0)) {
        const float halfWidth = 0.5 * width();
        const float halfHeight = 0.5 * height();
        const float radius = 1.8 * MAX(halfHeight, halfWidth);
        const float theta = asin(float(width()) / (2 * radius));
        const float rho = (PI * 0.5) - theta;
        const float centerX = halfWidth;
        const float centerY = height() - radius;

        float centAngle = (2 * theta) / 120;
        float note = rho + (fabs(needleValueToDraw - 60) * centAngle);
        int halfKnobWidth = MAX(toInt(radius * 0.02), 1);

        float noteX = centerX + radius * cos(note);
        float noteY = centerY + radius * sin(note);
        float knobLeftX = centerX - halfKnobWidth;
        float knobLeftY = centerY;
        float knobRightX = centerX + halfKnobWidth;
        float knobRightY = centerY;

        glNewList(needle, GL_COMPILE);

        // Inside of the needle
        glBegin(GL_TRIANGLES);
        glColor3ub(255, 0, 0);
        glVertex2f(noteX, noteY);
        glVertex2f(knobLeftX, knobLeftY);
        glVertex2f(knobRightX, knobRightY);
        glEnd();

        // Outside of the needle
        glBegin(GL_LINE_LOOP);
        glColor3ub(127, 0, 0);
        glVertex2f(noteX, noteY);
        glVertex2f(knobLeftX, knobLeftY);
        glVertex2f(knobRightX, knobRightY);
        glEnd();

        glEndList();
      }

      updateGL();
    }
  }
}

void VibratoTunerWidget::resetLeds()
{
  for (int i = 0; i < 8; i++) {
    emit(ledSet(i, false));
  }
}
