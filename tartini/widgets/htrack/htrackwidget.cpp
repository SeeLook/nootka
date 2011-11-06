/***************************************************************************
                          htrackwidget.cpp  -  description
                             -------------------
    begin                : Aug 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <QMouseEvent>
#include <QWheelEvent>

#include "htrackwidget.h"
#include "gdata.h"
#include "useful.h"
#include "channel.h"
#include "array1d.h"
#include "piano3d.h"
#include "musicnotes.h"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

HTrackWidget::HTrackWidget(QWidget *parent, const char *name)
  : QGLWidget(parent, name)
{
}

HTrackWidget::~HTrackWidget()
{
  delete piano3d;
}

void HTrackWidget::initializeGL()
{
  setPeakThreshold(0.05f);
  setDistanceAway(1500.0);
  setViewAngleVertical(-35.0);
  setViewAngleHorizontal(20.0);
  translateX = 0.0;
  translateY = -60.0;
  
  piano3d = new Piano3d();
  //theKeyboard3d = glGenLists(1);
  //glNewList(theKeyboard3d, GL_COMPILE);
  //drawKeyboard3d(7);
  //glEndList();
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glShadeModel(GL_FLAT);
  glEnable(GL_NORMALIZE);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  //glEnable(GL_LINE_SMOOTH);
  // Set up the rendering context, define display lists etc.:
  //QColor bg = colorGroup().background();
  //QColor bg = gdata->backgroundColor();
  //glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX, gCurrentMatrix);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  setLightSpecular(0.5, 0.5, 0.5);
  /*
  GLfloat light0Ambient[4] = { 0.4, 0.4, 0.4, 0.1 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
  GLfloat light0Diffuse[4] = { 0.9, 0.9, 0.9, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
  GLfloat light0Specular[4] = { 0.5, 0.5, 0.5, 1.0 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
  */
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void HTrackWidget::resizeGL( int w, int h )
{
  // setup viewport, projection etc.:
  glViewport( 0, 0, (GLint)w, (GLint)h );
/*
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum(-1.0, 1.0, -0.4, 0.4, 1.5, 3000);
  //gluPerspective (40.0, (double)w / (double)h, 100.0, 2000.0);
  gluPerspective (40.0, (double)w / (double)h, 1000.0, 10000.0);
  //glOrtho(-1.2, 1.2*double(w)/double(h), -1.2, 1.2, 0.1, 100.0);

  glMultMatrixf(gCurrentMatrix);
  glMatrixMode(GL_MODELVIEW);
*/
  /*
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  */
}

void HTrackWidget::paintGL()
{
  Channel *active = gdata->getActiveChannel();
  
  QColor bg = gdata->backgroundColor();
  glClearColor( double(bg.red()) / 256.0, double(bg.green()) / 256.0, double(bg.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double nearPlane = 100.0;
  double w2 = double(width()) / 2.0;
  double h2 = double(height()) / 2.0;
  double ratio = nearPlane / double(width());
  glFrustum((-w2-translateX) * ratio, (w2-translateX) * ratio, (-h2-translateY) * ratio, (h2-translateY) * ratio, nearPlane, 10000.0);
  //gluPerspective (40.0, (double)width() / (double)height(), 10.0, 10000.0);
  //glOrtho(-1.2, 1.2*double(w)/double(h), -1.2, 1.2, 0.1, 100.0);
  //glMultMatrixf(gCurrentMatrix);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //eye_angle += eye_spin;
  //gluLookAt(center_x + sin(eye_angle*PI/180.0)*eye_radius, eye_y, center_z - cos(eye_angle*PI/180)*eye_radius, center_x, 0.0, center_z, 0.0, 1.0, 0.0);
  double center_x = 0.0, center_z = 0.0;
  gluLookAt(0.0, 0.0/*_distanceAway*/, _distanceAway, center_x, 0.0, center_z, 0.0, 1.0, 0.0);
  glRotated(-viewAngleVertical(), 1.0, 0.0, 0.0);
  glRotated(viewAngleHorizontal(), 0.0, 1.0, 0.0);
  
  setLightPosition(0.0, 2000.0, -1600.0);
  setLightAmbient(0.4f, 0.4f, 0.4f);
  setLightDiffuse(0.9f, 0.9f, 0.9f);

  glPushMatrix();
  //glMultMatrixf(gCurrentMatrix);

  //glTranslatef(-0.5, -0.5, -0.5);//set cube centre at the origin
  double pianoWidth = piano3d->pianoWidth();
  glTranslatef(-pianoWidth/2.0, 0.0, 0.0);//set center of keyboard at origin
  
  //glCallList(theKeyboard3d);
  piano3d->setAllKeyStatesOff();
  //Channel *active = gdata->getActiveChannel();
  if(active) {
    AnalysisData *data = active->dataAtCurrentChunk();
    //Channel::getActiveChannelCurrentChunkData();
    //if(data && active->isVisibleNote(data->noteIndex)) {
    if(data && active->isVisibleChunk(data)) {
      piano3d->setMidiKeyState(toInt(data->pitch), true);
    }
  }
  glLineWidth(0.01f);
  piano3d->draw();

  setLightDirection(-1.0, 0.0, 0.0);
  setLightAmbient(0.2f, 0.2f, 0.2f);
  setLightDiffuse(0.9f, 0.9f, 0.9f);
  
  glTranslatef(-piano3d->firstKeyOffset, 0.0, 0.0);
  glScaled(OCTAVE_WIDTH / 12.0, 200.0, 5.0); //set a scale of 1 semitime = 1 unit
  
  glColor4f( 0.3f, 0.3f, 0.3f, 1.0f );
  glLineWidth(1.0);


  if(active) {
    active->lock();
    //int j;
    //unsigned int k;
    //int l;
    double pos;
    //FreqPair *temp;
    //double pos2;
    int j;
    uint numHarmonics = 40;
    int visibleChunks = 512; //128;
    //int currentChunk = active->currentChunk();
    //int firstChunk = bound(currentChunk-64, 0, active->totalChunks()-1);
    //int lastChunk = bound(currentChunk+64, 0, active->totalChunks());
    Array2d<float> pitches(numHarmonics, visibleChunks);
    Array2d<float> amps(numHarmonics, visibleChunks);
    
    AnalysisData *data;
    int finishChunk = active->currentChunk();
    int startChunk = finishChunk - visibleChunks;
    uint harmonic;
    int chunkOffset;
    
    //draw the time ref lines
    glBegin(GL_LINES);
    for(j=roundUp(startChunk, 16); j<finishChunk; j += 16) {
      if(active->isValidChunk(j)) {
        glVertex3f(piano3d->firstKey(), 0.0, double(j-finishChunk));
        glVertex3f(piano3d->firstKey()+piano3d->numKeys(), 0.0, double(j-finishChunk));
      }
    }
    glEnd();
    
    //build a table of frequencies and amplitudes for faster drawing
    for(chunkOffset=0; chunkOffset<visibleChunks; chunkOffset++) {
      data = active->dataAtChunk(startChunk + chunkOffset);
      if(data && data->harmonicFreq.size() > 0) {
        for(harmonic=0; harmonic < numHarmonics; harmonic++) {
          //freqs(harmonic, chunkOffset) = data->harmonicFreq[harmonic];
          pitches(harmonic, chunkOffset) = freq2pitch(data->harmonicFreq[harmonic]);
          amps(harmonic, chunkOffset) = data->harmonicAmp[harmonic];
        }
      } else {
        for(harmonic=0; harmonic < numHarmonics; harmonic++) {
          pitches(harmonic, chunkOffset) = 0.0;
          amps(harmonic, chunkOffset) = 0.0;
        }
      }
    }
    
    bool insideLine;
    float curPitch=0.0, curAmp=0.0, prevPitch, diffNote;
    
    //draw the outlines
    setMaterialSpecular(0.0, 0.0, 0.0, 0.0);
    setMaterialColor(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0);
    //printf("_peakThreshold = %f\n", _peakThreshold);
    for(harmonic=0; harmonic < numHarmonics; harmonic++) {
      insideLine = false;
      pos = -double(visibleChunks-1);
      for(chunkOffset=0; chunkOffset<visibleChunks; chunkOffset++, pos++) {
        curAmp = amps(harmonic, chunkOffset) - _peakThreshold;
        //printf("curAmp = %f\n", curAmp);
        if(curAmp > 0.0) {
          prevPitch = curPitch;
          curPitch = pitches(harmonic, chunkOffset);
          diffNote = prevPitch - curPitch;
          if(fabs(diffNote) < 1.0) {
            if(!insideLine) {
              glBegin(GL_LINE_STRIP);
              glVertex3f(curPitch, 0, pos);
              insideLine = true;
            }
            glVertex3f(curPitch, curAmp, pos);
          } else {
            glEnd();
            insideLine = false;
          }
        } else {
          if(insideLine) {
            glVertex3f(curPitch, 0, pos-1);
            glEnd();
            insideLine = false;
          }
        }
      }
      if(insideLine) {
        glVertex3f(curPitch, 0, pos-1);
        glEnd();
        insideLine = false;
      }
    }
    
    bool isEven;
    //draw the faces
    glShadeModel(GL_FLAT);
    //glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for(harmonic=0; harmonic < numHarmonics; harmonic++) {
      if(harmonic % 2 == 0) {
        isEven = true;
        //glColor4f( 0.5f, 0.5f, 0.9f, 0.2f );
        setMaterialColor(0.5f, 0.5f, 0.9f);
      } else {
        //glColor4f( 0.5f, 0.9f, 0.5f, 0.2f );
        setMaterialColor(0.5f, 0.9f, 0.5f);
        isEven = false;
      }
      insideLine = false;
      pos = -double(visibleChunks-1);
      for(chunkOffset=0; chunkOffset<visibleChunks; chunkOffset++, pos++) {
        if(amps(harmonic, chunkOffset) > _peakThreshold) {
          if(!insideLine) {
            glBegin(GL_QUAD_STRIP);
            insideLine = true;
            curPitch = pitches(harmonic, chunkOffset);
            glVertex3f(curPitch, (amps(harmonic, chunkOffset) - _peakThreshold), pos);
            glVertex3f(curPitch, 0, pos);
          } else {
            prevPitch = curPitch;
            curPitch = pitches(harmonic, chunkOffset);
            diffNote = prevPitch - curPitch;
            if(fabs(diffNote) < 1.0) {
              //setMaterialColor(0.5f * diffNote, ((isEven)?0.5f:0.9) * diffNote, ((isEven)?0.9f:0.5) * diffNote);
              glNormal3f(diffNote ,0.0 ,1.0);
              glVertex3f(curPitch, (amps(harmonic, chunkOffset) - _peakThreshold), pos);
              glVertex3f(curPitch, 0, pos);
            } else {
              glEnd();
              insideLine = false;
            }
          }
        } else {
          if(insideLine) { glEnd(); insideLine = false; }
        }
      }
      if(insideLine) { glEnd(); insideLine = false; }
    }
    active->unlock();
  }
  glPopMatrix();
}

void HTrackWidget::rotateX(double angle)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glLoadIdentity();
  glRotatef(angle, 1.0, 0.0, 0.0);
  glMultMatrixf(gCurrentMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, gCurrentMatrix);
  
  glPopMatrix();
}

void HTrackWidget::home()
{
  setPeakThreshold(0.05f);
  setDistanceAway(1500.0);
  setViewAngleVertical(-35.0);
  setViewAngleHorizontal(20.0);
  translateX = 0.0;
  translateY = -60.0;
}

void HTrackWidget::rotateY(double angle)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glLoadIdentity();
  glRotatef(angle, 0.0, 1.0, 0.0);
  glMultMatrixf(gCurrentMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, gCurrentMatrix);
  
  glPopMatrix();
}

void HTrackWidget::translate(float x, float y, float z)
{
  translateX += x;
  translateY += y;
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glLoadIdentity();
  glTranslatef(x, y, z);
  glMultMatrixf(gCurrentMatrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, gCurrentMatrix);
  
  glPopMatrix();
}

void HTrackWidget::mousePressEvent( QMouseEvent *e)
{
  mouseDown = true;
  mouseX = e->x();
  mouseY = e->y();
}

void HTrackWidget::mouseMoveEvent( QMouseEvent *e)
{
  if(mouseDown) {
    //rotateY((double)(e->x() - mouseX) * 0.2);
    //rotateX((double)(e->y() - mouseY) * 0.2);
    translate(float(e->x() - mouseX), -float(e->y() - mouseY), 0.0);

    mouseX = e->x();
    mouseY = e->y();
    update();
  }
}

void HTrackWidget::mouseReleaseEvent( QMouseEvent * )
{
  mouseDown = false;
}

void HTrackWidget::wheelEvent(QWheelEvent * e)
{
  setDistanceAway(_distanceAway * pow(2.0, -(double(e->delta()) / double(WHEEL_DELTA)) / 20.0));
  //_distanceAway *= pow(2.0, -(double(e->delta()) / double(WHEEL_DELTA)) / 20.0);
  update();
  e->accept();
}

