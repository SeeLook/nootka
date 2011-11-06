/***************************************************************************
                          piano3d.h  -  description
                             -------------------
    begin                : 31 March 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef PIANO3D_H
#define PIANO3D_H

#include <vector>
#include <stdio.h>
#include <qgl.h>
#include "myassert.h"

#define WHITE_KEY_WIDTH            23.5f
#define WHITE_KEY_LENGTH           148.0f
#define WHITE_KEY_LENGTH_INSIDE    142.0f
#define WHITE_KEY_HEIGHT           16.0f
#define BLACK_KEY_WIDTH            11.0f
#define BLACK_KEY_LENGTH_TOP       94.0f
#define BLACK_KEY_LENGTH_BOTTOM    100.0f
#define BLACK_KEY_HEIGHT           10.0f
#define OCTAVE_WIDTH               164.5f

class Piano3d
{
public:
  Piano3d(int numKeys_=85, int firstKey_=21);
  ~Piano3d();
  
  static void drawWhiteKey();
  //static void drawWhiteKeyLine();
  static void drawBlackKey();
  
  //static void drawBlackKeyAt(double x, bool isOn);
  //static void drawWhiteKeyAt(double x, bool isOn);
  void draw();
  
  void init(int numKeys_=85, int firstKey_=21);
  
  void setMaterialWhiteKey();
  void setMaterialBlackKey();

  
  double offsetAtKey(int keyNum);
  bool keyState(int keyNum) { return (keyNum >= 0 && keyNum < _numKeys) ? keyStates[keyNum] : false; }
  bool midiKeyState(int keyNum) { return keyState(keyNum-_firstKey); }
  void setKeyState(int keyNum, bool state) { if(keyNum >= 0 && keyNum < _numKeys) keyStates[keyNum] = state; }
  void setMidiKeyState(int keyNum, bool state) { setKeyState(keyNum-_firstKey, state); }
  void setAllKeyStatesOff();

  double pianoWidth();
  int numKeys() { return _numKeys; }
  int firstKey() { return _firstKey; }
  double firstKeyOffset;
  
private:
  std::vector<bool> keyStates;
  std::vector<float> keyOffsets;
  int _numKeys;
  int _firstKey; //on the midi scale (ie middle C = 60)
  GLint aWhiteKey; //, aWhiteKeyLine;
  GLint aBlackKey;
};

inline void setMaterialColor(float r, float g, float b) {
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, theColor);
}

inline void setMaterialSpecular(float r, float g, float b, float shiney) {
  GLfloat theSpecular[4] = { r, g, b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, theSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiney);
}

inline void setLightPosition(float x, float y, float z)
{
  GLfloat light_position[4] = { x, y, z, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

inline void setLightDirection(float x, float y, float z)
{
  GLfloat light_position[4] = { x, y, z, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

inline void setLightAmbient(float r, float g, float b) {
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, theColor);
}

inline void setLightDiffuse(float r, float g, float b) {
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, theColor);
}

inline void setLightSpecular(float r, float g, float b) {
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, theColor);
}

#endif
