/***************************************************************************
                          piano3d.cpp  -  description
                             -------------------
    begin                : 31 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <algorithm>

#include "piano3d.h"
#include "useful.h"
#include "musicnotes.h"

double keyOffsetTable[12] = { 0.0, 15.5, 23.5, 43.5, 47.0, 70.5, 85.5, 94.0, 112.5, 117.5, 139.0, 141.0 };

Piano3d::Piano3d(int numKeys_, int firstKey_)
{
  init(numKeys_, firstKey_);
}

Piano3d::~Piano3d()
{
}

/**
@return The width of the current piano keyboard in mm
*/
double Piano3d::pianoWidth()
{
  return (isBlackNote(_numKeys-1)+_firstKey) ? keyOffsets[_numKeys-1]+BLACK_KEY_WIDTH : keyOffsets[_numKeys-1]+WHITE_KEY_WIDTH;
}

/**
@return The distance the in mm away from where the 0 midi note would be
*/
double Piano3d::offsetAtKey(int keyNum)
{
  return (noteOctave(keyNum)+1)*OCTAVE_WIDTH + keyOffsetTable[cycle(keyNum, 12)] - firstKeyOffset;
}

/** Initialises the piano.
This can be called again to reinitialise the piano to a different size
@param numKeys_ The number of keys the piano is to have
@param firstKey_ The midi note number that will be the lowest key on the piano
*/
void Piano3d::init(int numKeys_, int firstKey_)
{
  _numKeys = numKeys_;
  _firstKey = firstKey_;
  keyStates.resize(_numKeys, false);
  keyOffsets.resize(_numKeys);
  firstKeyOffset = (noteOctave(_firstKey)+1)*OCTAVE_WIDTH + keyOffsetTable[cycle(_firstKey, 12)];
  int curKey;
  for(int j=0; j<_numKeys; j++) {
    curKey = j+_firstKey;
    keyOffsets[j] = offsetAtKey(curKey);
  }
  
  //build display lists for a white and a black key (for fast rendering later)
  aWhiteKey = glGenLists(1);
  glNewList(aWhiteKey, GL_COMPILE);
  drawWhiteKey();
  glEndList();
  //aWhiteKeyLine = glGenLists(1);
  //glNewList(aWhiteKeyLine, GL_COMPILE);
  //drawWhiteKeyLine();
  //glEndList();
  aBlackKey = glGenLists(1);
  glNewList(aBlackKey, GL_COMPILE);
  drawBlackKey();
  glEndList();
}

void Piano3d::setAllKeyStatesOff()
{
  std::fill(keyStates.begin(), keyStates.end(), false);
}

void Piano3d::drawWhiteKey()
{
  glBegin(GL_QUAD_STRIP);
  //top surface
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(WHITE_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  
  glVertex3f(WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
  glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH);
  
  //front surface
  glNormal3f(0.0f, -1.0f, 0.2f); //todo: fix normal
  glVertex3f(WHITE_KEY_WIDTH, -2.0f, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(0.0f, -2.0f, WHITE_KEY_LENGTH_INSIDE);
  
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(0.0f, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glEnd();
}
/*
void Piano3d::drawWhiteKeyLine()
{
  glBegin(GL_QUADS);
  //top surface
  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(WHITE_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
  glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH);
  glEnd();
  //front surface
  glBegin(GL_LINE_STRIP);
  glNormal3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(0.0, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH_INSIDE);
  glEnd();
}
*/
void Piano3d::drawBlackKey()
{
  glBegin(GL_QUAD_STRIP);
  glNormal3f(-1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glNormal3f(1.0, 0.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
  
  glBegin(GL_QUADS);
  glNormal3f(0.0, 3.0, 1.0); //todo: fix normal
  glVertex3f(BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(0.0, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
}

/*
void Piano3d::drawBlackKeyAt(double x, bool isOn)
{
  if(isOn) {
    glColor3f(0.5, 0.5, 0.0);
    glPushMatrix();
    glRotated(5.0, 1.0, 0.0, 0.0);
  } else {
    glColor3f(0.0, 0.0, 0.0);
  }
  glBegin(GL_QUAD_STRIP);
  glVertex3f(x, 0.0, 0.0);
  glVertex3f(x, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(x, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(x, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(x+BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(x+BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(x+BLACK_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(x+BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(x, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(x, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(x+BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(x+BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
  if(isOn) glPopMatrix();
}

void Piano3d::drawWhiteKeyAt(double x, bool isOn)
{
  if(isOn) {
    glColor3f(1.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(5.0, 1.0, 0.0, 0.0);
  } else {
    glColor3f(1.0, 1.0, 1.0);
  }
  glBegin(GL_QUADS);
  double delta = 1.0;
  //top surface
  glVertex3f(x, -delta, 0.0);
  glVertex3f(x+WHITE_KEY_WIDTH, -delta, 0.0);
  glVertex3f(x+WHITE_KEY_WIDTH, -delta, WHITE_KEY_LENGTH);
  glVertex3f(x, -delta, WHITE_KEY_LENGTH);
  //front surface
  glVertex3f(x, -delta, WHITE_KEY_LENGTH_INSIDE-delta);
  glVertex3f(x+WHITE_KEY_WIDTH, -delta, WHITE_KEY_LENGTH_INSIDE-delta);
  glVertex3f(x+WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE-delta);
  glVertex3f(x, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE-delta);
  glEnd();
  //draw balck outline around key
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINE_LOOP);
  glVertex3f(x, 0.0, 0.0);
  glVertex3f(x+WHITE_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(x+WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
  glVertex3f(x, 0.0, WHITE_KEY_LENGTH);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex3f(x, -2*delta, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(x, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(x+WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(x+WHITE_KEY_WIDTH, -2*delta, WHITE_KEY_LENGTH_INSIDE);
  glEnd();
  if(isOn) glPopMatrix();
}
*/

//units are in mm
/*void Piano3d::draw(int numOctaves)
{
  int j;
  //draw white keyboard surface
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(0.0, -1, 0.0);
  glVertex3f(numOctaves*164.5, -1, 0.0);
  glVertex3f(0.0, -1, WHITE_KEY_LENGTH);
  glVertex3f(numOctaves*164.5, -1, WHITE_KEY_LENGTH);
  glEnd();

  //draw black outline to keyboard
  glLineWidth(1.0);
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINE_LOOP);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(numOctaves*164.5, 0.0, 0.0);
  glVertex3f(numOctaves*164.5, 0.0, WHITE_KEY_LENGTH);
  glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH);
  glEnd();

  //draw black lines between white keys
  glBegin(GL_LINES);
  for(j=1; j<numOctaves*7; j++) {
    glVertex3f(double(j) * WHITE_KEY_WIDTH, 0.0, 0.0);
    glVertex3f(double(j) * WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
  }
  glEnd();

  double blackKeyLookup[5] = { 21.5, 62.5, 90.5, 132.5, 159.5 };
  //draw black keys
  for(int octave=0; octave<numOctaves; octave++) {
    for(int blackNote=0; blackNote<((octave<numOctaves-1)?5:4); blackNote++) {
      drawBlackKeyAt(double(octave)*164.5 + blackKeyLookup[blackNote], true);
    }  
  }
}
*/
void Piano3d::setMaterialWhiteKey()
{
  GLfloat pianoKeyDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pianoKeyDiffuse);
  //GLfloat pianoKeyAmbient[] = { 0.7, 0.7, 0.7, 1.0 };
  //glMaterialfv(GL_FRONT, GL_AMBIENT, pianoKeyAmbient);
  //GLfloat pianoKeySpecular[] = { 0.0, 0.0, 0.0, 0.0 };
  //glMaterialfv(GL_FRONT, GL_SPECULAR, pianoKeySpecular);
  //GLfloat pianoKeyShininess[] = { 50.0 };
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pianoKeyShininess);
}

void Piano3d::setMaterialBlackKey()
{
  GLfloat pianoKeyDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pianoKeyDiffuse);
  //GLfloat pianoKeyAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
  //glMaterialfv(GL_FRONT, GL_AMBIENT, pianoKeyAmbient);
  //GLfloat pianoKeySpecular[] = { 0.3, 0.3, 0.3, 1.0 };
  //glMaterialfv(GL_FRONT, GL_SPECULAR, pianoKeySpecular);
  //GLfloat pianoKeyShininess[] = { 50.0 };
  //glMaterialfv(GL_FRONT, GL_SHININESS, pianoKeyShininess);
}

/*
void Piano3d::draw()
{
  int curKey;
  for(int j=0; j<numKeys; j++) {
    curKey = j+firstKey;
    if(isBlackNote(curKey)) drawBlackKeyAt(keyOffsets[j], keyStates[j]);
    else drawWhiteKeyAt(keyOffsets[j], keyStates[j]);
  }
}
*/

void Piano3d::draw()
{
  int curKey;
  int j;

  glShadeModel(GL_FLAT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPolygonMode(GL_FRONT, GL_FILL);
  
  //draw white keys filled
  setMaterialSpecular( 0.2f, 0.2f, 0.2f, 50.0f);
  setMaterialColor(0.9f, 0.9f, 0.9f);
  glPushMatrix();
  //glColor3f(1.0, 1.0, 1.0);
  for(j=0; j<_numKeys; j++) {
    curKey = j+_firstKey;
    if(!isBlackNote(curKey)) {
      if(keyStates[j]) {
        //glColor3f(1.0, 1.0, 0.0);
        setMaterialColor(0.9f, 0.9f, 0.0f);
        glPushMatrix();
        glRotated(5.0, 1.0, 0.0, 0.0);
        glCallList(aWhiteKey);
        glPopMatrix();
        //glColor3f(1.0, 1.0, 1.0);
        setMaterialColor(0.9f, 0.9f, 0.9f);
      } else {
        glCallList(aWhiteKey);
      }
      glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
    }
  }
  glPopMatrix();
  
  //draw black keys filled
  setMaterialColor(0.15f, 0.15f, 0.15f);
  for(j=0; j<_numKeys; j++) {
    curKey = j+_firstKey;
    if(isBlackNote(curKey)) {
      glPushMatrix();
      glTranslatef(keyOffsets[j], 0.0, 0.0);
      if(keyStates[j]) {
        glRotated(5.0, 1.0, 0.0, 0.0);
        setMaterialColor(0.5, 0.5, 0.0);
        glCallList(aBlackKey);
        setMaterialColor(0.15f, 0.15f, 0.15f);
      } else {
        glCallList(aBlackKey);
      }
      glPopMatrix();
    }
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  
  //draw white key outline
  setMaterialColor(0.0, 0.0, 0.0);
  glLineWidth(1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glPushMatrix();
  for(j=0; j<_numKeys; j++) {
    curKey = j+_firstKey;
    if(!isBlackNote(curKey)) {
      if(keyStates[j]) {
        glPushMatrix();
        glRotated(5.0, 1.0, 0.0, 0.0);
        glCallList(aWhiteKey);
        glPopMatrix();
      } else {
        glCallList(aWhiteKey);
      }
      glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
    }
  }
  glPopMatrix();

  //draw black key outline
  setMaterialColor(0.35f, 0.35f, 0.35f);
  for(j=0; j<_numKeys; j++) {
    curKey = j+_firstKey;
    if(isBlackNote(curKey)) {
      glPushMatrix();
      glTranslatef(keyOffsets[j], 0.0, 0.0);
      if(keyStates[j]) {
        glRotated(5.0, 1.0, 0.0, 0.0);
        //setMaterialColor3f(0.5, 0.5, 0.0);
        glCallList(aBlackKey);
        //setMaterialColor3f(0.35, 0.35, 0.35);
      } else {
        glCallList(aBlackKey);
      }
      glPopMatrix();
    }
  }
}
