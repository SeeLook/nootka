/***************************************************************************
                          vibratospeedwidget.cpp  -  description
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
#include "vibratospeedwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"

VibratoSpeedWidget::VibratoSpeedWidget(QWidget *parent)
  : QGLWidget(parent)
{
  speedValueToDraw = 0;
  widthValueToDraw = 0;
  prevVibratoSpeed = 0;
  prevVibratoWidth = 0;
  useProny = true;
  widthLimit = 50;
  prevNoteNumber = -1;
  speedLabelCounter = 0;
  widthLabelCounter = 0;
  for (int i = 0; i < 100; i++) {
    speedLabels[i].label = QString(8,' ');
    speedLabels[i].x = 0.0f;
    speedLabels[i].y = 0.0f;
    widthLabels[i].label = QString(8,' ');
    widthLabels[i].x = 0.0f;
    widthLabels[i].y = 0.0f;
  }
  speedWidthFont = QFont();
  speedWidthFont.setPointSize(9);
}

VibratoSpeedWidget::~VibratoSpeedWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(speedDial, 1);
  glDeleteLists(speedNeedle, 1);
  glDeleteLists(widthDial, 1);
  glDeleteLists(widthNeedle, 1);

}

void VibratoSpeedWidget::initializeGL()
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

  speedDial = glGenLists(1);
  speedNeedle = glGenLists(1);
  widthDial = glGenLists(1);
  widthNeedle = glGenLists(1);
}

void VibratoSpeedWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  //glMatrixMode(GL_MODELVIEW);

  const float halfWidth = 0.5 * width();
  const float halfHeight = 0.5 * height();
  const float radius = 1.8 * MAX(halfHeight, halfWidth);
  const float theta = asin(float(width()) / (2 * radius));
  const float rho = (PI / 2.0) - theta;


  // Calculate the speed dial with the new width & height
  const float speedCenterX = halfWidth;
  const float speedCenterY = height() - radius;

  glNewList(speedDial, GL_COMPILE);

  // Calculate the coordinates of the points on the upper arc
  float *speedUpperArcX, *speedUpperArcY;
  speedUpperArcX = new float[width()];
  speedUpperArcY = new float[width()];
  float speedUpperArcTheta = -theta;
  float speedUpperArcThetaStep = (2 * theta) / width();
  for (int x = 0; x < width(); x++) {
    speedUpperArcX[x] = x;
    speedUpperArcY[x] = speedCenterY + (sin(0.5*PI - speedUpperArcTheta) * radius);
    speedUpperArcTheta += speedUpperArcThetaStep;
  }

  // Calculate the white semicircle of the dial
  glBegin(GL_TRIANGLE_FAN);
  glColor3ub(255, 255, 255);
  glVertex2f(speedCenterX, speedCenterY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(speedUpperArcX[x], speedUpperArcY[x]);
  }
  glEnd();

  // Calculate the black outline of the dial
  glBegin(GL_LINE_LOOP);
  glColor3ub(0, 0, 0);
  glVertex2f(speedCenterX, speedCenterY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(speedUpperArcX[x], speedUpperArcY[x]);
  }
  glEnd();

  // Calculate the black arc inside the dial
  glBegin(GL_LINE_STRIP);
  glColor3ub(0, 0, 0);
  float speedLowerArcTheta = -theta;
  float speedLowerArcThetaStep = (2 * theta) / halfWidth;
  for (float x = 0.25 * width(); x < 0.75 * width(); x++) {
    glVertex2f(x, speedCenterY + (0.5 * sin(0.5*PI - speedLowerArcTheta) * radius));
    speedLowerArcTheta += speedLowerArcThetaStep;
  }
  glEnd();

  // Calculate line markings & text labels
  float speedStep = (2 * theta) / 14.0;
  speedLabelCounter = 0;
  const char *speedLabelLookup[13] = { "12   ", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "0" };

  for (int j=0; j<13; j++) {
    float i = rho + speedStep * (j+1);
    float startX = speedCenterX + radius * cos(i);
    float startY = speedCenterY + radius * sin(i);
    if (width() < 175) {  // Small dial
      if ((j % 4) == 0) {
        // Bigger marking + text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.05 * (speedCenterX - startX), startY + 0.05 * (speedCenterY - startY));
        glEnd();

        speedLabels[speedLabelCounter].x = startX + 0.08 * (speedCenterX - startX);
        speedLabels[speedLabelCounter].y = startY + 0.08 * (speedCenterY - startY);
        speedLabels[speedLabelCounter++].label = speedLabelLookup[j];
      } else {
        // Smaller marking, no text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.03 * (speedCenterX - startX), startY + 0.03 * (speedCenterY - startY));
        glEnd();
      }
    } else if (width() < 300) {  // Bigger dial
      if ((j % 2) == 0) {
        // Bigger marking + text label
        speedLabelLookup[0] = "12";

        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.05 * (speedCenterX - startX), startY + 0.05 * (speedCenterY - startY));
        glEnd();

        speedLabels[speedLabelCounter].x = startX + 0.08 * (speedCenterX - startX);
        speedLabels[speedLabelCounter].y = startY + 0.08 * (speedCenterY - startY);
        speedLabels[speedLabelCounter++].label = speedLabelLookup[j];
      } else {
        // Smaller marking, no text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.03 * (speedCenterX - startX), startY + 0.03 * (speedCenterY - startY));
        glEnd();
      }
    } else {  // Big dial
      // Bigger marking + text label
      speedLabelLookup[0] = "12";

      glBegin(GL_LINES);
      glColor3ub(0, 0, 0);
      glVertex2f(startX, startY);
      glVertex2f(startX + 0.05 * (speedCenterX - startX), startY + 0.05 * (speedCenterY - startY));
      glEnd();

      speedLabels[speedLabelCounter].x = startX + 0.08 * (speedCenterX - startX);
      speedLabels[speedLabelCounter].y = startY + 0.08 * (speedCenterY - startY);
      speedLabels[speedLabelCounter++].label = speedLabelLookup[j];
    }
  }

  // Calculate the "Hz" label
  hzLabelX = speedCenterX;
  hzLabelY = speedCenterY + 0.7 * radius;

  glEndList();

  delete[] speedUpperArcX;
  delete[] speedUpperArcY;


  // Calculate the width dial with the new width & height
  const float widthCenterX = halfWidth;
  const float widthCenterY = halfHeight - radius;

  glNewList(widthDial, GL_COMPILE);

  // Calculate the openGL-coordinates of the points on the upper arc
  float *widthUpperArcX, *widthUpperArcY;
  widthUpperArcX = new float[width()];
  widthUpperArcY = new float[width()];
  float widthUpperArcTheta = -theta;
  float widthUpperArcThetaStep = (2 * theta) / width();
  for (int x = 0; x < width(); x++) {
    widthUpperArcX[x] = x;
    widthUpperArcY[x] = widthCenterY + (sin(0.5*PI - widthUpperArcTheta) * radius);
    widthUpperArcTheta += widthUpperArcThetaStep;
  }

  // Calculate the white semicircle of the dial
  glBegin(GL_TRIANGLE_FAN);
  glColor3ub(255, 255, 255);
  glVertex2f(widthCenterX, widthCenterY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(widthUpperArcX[x], widthUpperArcY[x]);
  }
  glEnd();

  // Calculate the black outline of the dial
  glBegin(GL_LINE_LOOP);
  glColor3ub(0, 0, 0);
  glVertex2f(widthCenterX, widthCenterY);
  for (int x = 0; x < width(); x++) {
    glVertex2f(widthUpperArcX[x], widthUpperArcY[x]);
  }
  glEnd();

  // Calculate the black arc inside the dial
  glBegin(GL_LINE_STRIP);
  glColor3ub(0, 0, 0);
  float widthLowerArcTheta = -theta;
  float widthLowerArcThetaStep = (2 * theta) / halfWidth;
  for (float x = 0.25 * width(); x < 0.75 * width(); x++) {
    glVertex2f(x, widthCenterY + (0.5 * sin(0.5*PI - widthLowerArcTheta) * radius));
    widthLowerArcTheta += widthLowerArcThetaStep;
  }
  glEnd();

  // Calculate line markings & text labels
  float widthStep = (2 * theta) / 12.0;
  widthLabelCounter = 0;
  char widthLabel[5];

  for (int j=0; j<11; j++) {
    float i = rho + widthStep * (j+1);
    float startX = widthCenterX + radius * cos(i);
    float startY = widthCenterY + radius * sin(i);
    if (width() < 250) {
      if ((j % 2) == 0) {
        // Bigger marking + text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.05 * (widthCenterX - startX), startY + 0.05 * (widthCenterY - startY));
        glEnd();

        widthLabels[widthLabelCounter].x = startX + 0.08 * (widthCenterX - startX);
        widthLabels[widthLabelCounter].y = startY + 0.08 * (widthCenterY - startY);
        sprintf(widthLabel, "%d", widthLimit - ((widthLimit/10)*j));
        widthLabels[widthLabelCounter++].label = widthLabel;
      } else {
        // Smaller marking, no text label
        glBegin(GL_LINES);
        glColor3ub(0, 0, 0);
        glVertex2f(startX, startY);
        glVertex2f(startX + 0.03 * (widthCenterX - startX), startY + 0.03 * (widthCenterY - startY));
        glEnd();
      }
    } else {
      // Bigger marking + text label
      glBegin(GL_LINES);
      glColor3ub(0, 0, 0);
      glVertex2f(startX, startY);
      glVertex2f(startX + 0.05 * (widthCenterX - startX), startY + 0.05 * (widthCenterY - startY));
      glEnd();

      widthLabels[widthLabelCounter].x = startX + 0.08 * (widthCenterX - startX);
      widthLabels[widthLabelCounter].y = startY + 0.08 * (widthCenterY - startY);
      sprintf(widthLabel, "%d", widthLimit - ((widthLimit/10)*j));
      widthLabels[widthLabelCounter++].label = widthLabel;
    }
  }

  // Calculate the "Cents" label
  centsLabelX = widthCenterX;
  centsLabelY = widthCenterY + 0.7 * radius;

  glEndList();

  delete[] widthUpperArcX;
  delete[] widthUpperArcY;


  // Do forced update on resize
  prevVibratoSpeed = -999;
  prevVibratoWidth = -999;
  doUpdate();
}

void VibratoSpeedWidget::paintGL()
{
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  QFontMetrics fm = QFontMetrics(speedWidthFont);

  // Draw the speed dial
  glLineWidth(1.5);
  glCallList(speedDial);

  // Draw the labels for the speed dial
  glColor3ub(0,0,0);
  renderText(hzLabelX - (0.5 * fm.width("Hz")), hzLabelY, 0, "Hz", speedWidthFont);
  for (int i = 0; i < speedLabelCounter; i++) {
    renderText(speedLabels[i].x - (0.5 * fm.width(speedLabels[i].label)), speedLabels[i].y - 8, 0, speedLabels[i].label, speedWidthFont);
  }

  // Draw the speed needle
  glLineWidth(1.0);
  glCallList(speedNeedle);

  // Draw the width dial
  glLineWidth(1.5);
  glCallList(widthDial);

  // Draw the labels for the width dial
  glColor3ub(0,0,0);
  renderText(centsLabelX - (0.5 * fm.width("Cents")), centsLabelY, 0, "Cents", speedWidthFont);
  for (int i = 0; i < widthLabelCounter; i++) {
    renderText(widthLabels[i].x - (0.5 * fm.width(widthLabels[i].label)), widthLabels[i].y - 8, 0, widthLabels[i].label, speedWidthFont);
  }

  // Draw the width needle
  glLineWidth(1.0);
  glCallList(widthNeedle);
}

void VibratoSpeedWidget::doUpdate()
{
  Channel *active = gdata->getActiveChannel();

  float vibratoSpeed = 0;
  float vibratoWidth = 0;
  int currentNoteNumber = -1;

  if(active) {
  //if ((active) && (active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
    AnalysisData *data;
    if(gdata->soundMode & SOUND_REC) data = active->dataAtChunk(active->chunkAtCurrentTime() - active->pronyDelay());
    else data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) {
      NoteData *note = new NoteData();
      note = &(active->noteData[data->noteIndex]);

      currentNoteNumber = data->noteIndex;
      if (useProny) {
        vibratoSpeed = data->vibratoSpeed;
        vibratoWidth = 200 * data->vibratoWidth;
      } else if((active->doingDetailedPitch()) && (active->pitchLookupSmoothed.size() > 0)) {
        large_vector<float> pitchLookupUsed = active->pitchLookupSmoothed;
        int smoothDelay = active->pitchBigSmoothingFilter->delay();

        int currentTime = active->chunkAtCurrentTime() * active->framesPerChunk() + smoothDelay;
        int maximumTime = 0;
        int minimumTime = 0;
        int maximaSize = note->maxima->size();
        int minimaSize = note->minima->size();

        // Find the most recent maximum
        for (int i = 1; i < maximaSize; i++) {
          if ((currentTime > note->maxima->at(i-1)) && (currentTime <= note->maxima->at(i))) {
            maximumTime = note->maxima->at(i-1);
            break;
          }
        }
        if ((maximaSize > 0) && (currentTime > note->maxima->at(maximaSize - 1))) {
          maximumTime = note->maxima->at(maximaSize - 1);
        }

        // Find the most recent minimum
        for (int i = 1; i < note->minima->size(); i++) {
          if ((currentTime > note->minima->at(i-1)) && (currentTime <= note->minima->at(i))) {
            minimumTime = note->minima->at(i-1);
            break;
          }
        }
        if ((note->minima->size() > 0) && (currentTime > note->minima->at(minimaSize - 1))) {
          minimumTime = note->minima->at(minimaSize - 1);
        }

        if ((maximumTime !=0 ) && (minimumTime != 0)) {  // The speed and width can be calculated
          vibratoSpeed = active->rate() / (float)(2 * abs(maximumTime - minimumTime));
          vibratoWidth = fabs(100 * (pitchLookupUsed.at(maximumTime) - pitchLookupUsed.at(minimumTime)));
        }
      }
    }
  }

  if ((fabs(prevVibratoSpeed - vibratoSpeed) < 0.05) && (fabs(prevVibratoWidth - vibratoWidth) < 0.05)) {
    // Needle values haven't changed (much) , no update needed

  } else {
    // Needle values have changed

    makeCurrent();

    glNewList(speedNeedle, GL_COMPILE);
    glEndList();

    glNewList(widthNeedle, GL_COMPILE);
    glEndList();

    if ((vibratoSpeed == 0) && (vibratoWidth == 0)) {
      // No needle values, don't draw the needles this update

      prevVibratoSpeed = 0;
      prevVibratoWidth = 0;

      speedValueToDraw = 0;
      widthValueToDraw = 0;
      prevNoteNumber = -1;

      updateGL();

    } else {
      // Needle values, draw the needles this update

      prevVibratoSpeed = vibratoSpeed;
      prevVibratoWidth = vibratoWidth;

      speedValueToDraw = vibratoSpeed;
      widthValueToDraw = vibratoWidth;

      const float halfWidth = 0.5 * width();
      const float halfHeight = 0.5 * height();
      const float radius = 1.8 * MAX(0.5 * height(), halfWidth);
      const float theta = asin(float(width()) / (2 * radius));
      const float rho = (PI * 0.5) - theta;
      const int halfKnobWidth = MAX(toInt(radius * 0.02), 1);

      // Calculate the speed needle, if speedValueToDraw is set to sensible value
      if ((speedValueToDraw > 0) && (speedValueToDraw <= 12)) {
        const float centerX = halfWidth;
        const float centerY = height() - radius;

        float hzAngle = (2 * theta) / 14.0;
        float note = PI - rho - (speedValueToDraw * hzAngle) - hzAngle;

        float noteX = centerX + radius * cos(note);
        float noteY = centerY + radius * sin(note);
        float knobLeftX = centerX - halfKnobWidth;
        float knobLeftY = centerY;
        float knobRightX = centerX + halfKnobWidth;
        float knobRightY = centerY;

        glNewList(speedNeedle, GL_COMPILE);

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

      // Determine whether the scale needs adjusting
      int oldWidthLimit = widthLimit;
      if ((widthValueToDraw > 0) && (currentNoteNumber == prevNoteNumber)) {
        if (widthValueToDraw > 50) {
          widthLimit = std::max(widthLimit, 100);
        }
        if (widthValueToDraw > 100) {
          widthLimit = std::max(widthLimit, 200);
        }
        if (widthValueToDraw > 200) {
          widthLimit = std::max(widthLimit, 300);
        }
      }
      if (prevNoteNumber != currentNoteNumber) {
        widthLimit = 50;
      }
      if (widthLimit != oldWidthLimit) {
        // Do resize and redraw the dial if the scale is adjusted
        resizeGL(width(), height());
      }
      prevNoteNumber = currentNoteNumber;

      // Calculate the width needle, if widthValueToDraw is set to sensible value
      if ((widthValueToDraw > 0) && (widthValueToDraw <= widthLimit)) {
        const float centerX = halfWidth;
        const float centerY = halfHeight - radius;

        float centAngle = (2 * theta) / (widthLimit + 2 * (widthLimit/10));
        float note = PI - rho - (widthValueToDraw * centAngle) - (widthLimit/10) * centAngle;
        int halfKnobWidth = MAX(toInt(radius * 0.02), 1);

        float noteX = centerX + radius * cos(note);
        float noteY = centerY + radius * sin(note);
        float knobLeftX = centerX - halfKnobWidth;
        float knobLeftY = centerY;
        float knobRightX = centerX + halfKnobWidth;
        float knobRightY = centerY;

        glNewList(widthNeedle, GL_COMPILE);

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

void VibratoSpeedWidget::setUseProny(bool value)
{
  useProny = value;
  prevVibratoSpeed = -999;
  prevVibratoWidth = -999;
  doUpdate();
}
