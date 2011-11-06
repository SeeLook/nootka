#ifndef MYGL_H
#define MYGL_H

#include <QGLWidget>

struct MyGLfloat2d {
  GLfloat x, y;
  MyGLfloat2d(GLfloat _x, GLfloat _y) : x(_x), y(_y) { }
  void set(GLfloat _x, GLfloat _y) { x=_x; y=_y; }
};

inline void mygl_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

inline void mygl_rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h)
{
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x+w, y);
  glVertex2f(x+w, y+h);
  glVertex2f(x, y+h);
  glEnd();
}

inline void mygl_rect(const MyGLfloat2d &v1, const MyGLfloat2d &v2)
{
  glBegin(GL_QUADS);
  glVertex2f(v1.x, v1.y);
  glVertex2f(v2.x, v1.y);
  glVertex2f(v2.x, v2.y);
  glVertex2f(v1.x, v2.y);
  glEnd();
}

inline void mygl_resize2d(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
}

inline GLushort my_wrap_left(GLushort x, int offset)
{
  offset %= 16;
  return (x << offset) + (x >> (16-offset));
}

inline GLushort my_wrap_right(GLushort x, int offset)
{
  offset %= 16;
  return (x >> offset) + (x << (16-offset));
}

#endif
