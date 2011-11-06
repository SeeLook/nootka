#ifndef MYGLFONTS_H
#define MYGLFONTS_H

#include <QGLWidget>
#include <QString>

/** 
  Simple use of MyGLFont

  //in initialiseGL()
  MyGLFont myglf(QFont());
  //in paintGL()
  myglf.drawGLtext(10, 10, "Hello World");
  
  
  To draw lots of text faster, use

  //in initialiseGL()
  MyGLFont myglf(QFont());
  //in paintGL()
  myglf.beginGLtext(width(), height());
  for(int j=1; j<100; j++) {
    myglf.drawGLtextRaw(10, 10*j, "Hello World");
  }
  myglf.endGLtext();
*/

class MyGLChar
{
  int _w, _h, _w_raw;
  GLubyte *_data;
public:
  MyGLChar(int width, int height, GLubyte *theData, int width_raw) : _w(width), _h(height), _w_raw(width_raw), _data(theData) { }
  GLubyte *data() { return _data; }
  int w() { return _w; }
  int h() { return _h; }
  int w_raw() { return _w_raw; }
};

class MyGLFont
{
  MyGLChar *c[256];
  bool beenInit;

public:
  MyGLFont() { beenInit = false; }
  MyGLFont(const QFont f) { init(f); }
  ~MyGLFont();
  void init(const QFont f);
  void drawGLtext(float x, float y, QString s, int w, int h);
  void beginGLtext(int w, int h);
  void drawGLtextRaw(float x, float y, QString s);
  void endGLtext();
};

#endif
